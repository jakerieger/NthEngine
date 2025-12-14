// JobSystem.cpp
/*
 *  Filename: JobSystem.cpp
 *  This code is part of the Astera core library
 *  Copyright 2025 Jake Rieger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "JobSystem.hpp"
#include "Log.hpp"

namespace Astera {
    // Global job system instance
    unique_ptr<JobSystem> gJobSystem;

    JobSystem::~JobSystem() {
        Shutdown();
    }

    void JobSystem::Initialize(size_t numThreads) {
        if (mInitialized.exchange(true)) {
            Log::Warn("JobSystem", "Already initialized");
            return;
        }

        // Use hardware concurrency if not specified
        if (numThreads == 0) {
            numThreads = std::thread::hardware_concurrency();
            if (numThreads == 0)
                numThreads = 4;  // Fallback
        }

        mShutdown = false;
        mWorkers.resize(numThreads);

        // Create worker threads
        for (size_t i = 0; i < numThreads; ++i) {
            mWorkers[i].thread = std::thread([this, i]() { WorkerLoop(i); });

            // Store thread ID for GetCurrentWorkerID
            {
                std::lock_guard<std::mutex> lock(mThreadMapMutex);
                mWorkers[i].threadId                         = mWorkers[i].thread.get_id();
                mThreadIdToWorkerIndex[mWorkers[i].threadId] = i;
            }
        }

        Log::Info("JobSystem", "Initialized with {} worker threads", numThreads);
    }

    void JobSystem::Shutdown() {
        if (!mInitialized.exchange(false)) {
            return;
        }

        Log::Info("JobSystem", "Shutting down job system...");

        // Signal shutdown
        mShutdown = true;

        // Wake up all workers
        mGlobalCondition.notify_all();

        // Join all worker threads
        for (auto& worker : mWorkers) {
            if (worker.thread.joinable()) {
                worker.thread.join();
            }
        }

        // Clear worker data
        mWorkers.clear();

        {
            std::lock_guard<std::mutex> lock(mThreadMapMutex);
            mThreadIdToWorkerIndex.clear();
        }

        // Clear any remaining jobs
        {
            std::lock_guard<std::mutex> lock(mGlobalMutex);
            while (!mGlobalQueue.empty()) {
                mGlobalQueue.pop();
            }
        }

        Log::Info("JobSystem",
                  "Shutdown complete. Total jobs: submitted={}, completed={}",
                  mTotalJobsSubmitted.load(),
                  mTotalJobsCompleted.load());
    }

    void JobSystem::Submit(Job job) {
        if (!mInitialized) {
            Log::Error("JobSystem", "Cannot submit job - system not initialized");
            return;
        }

        mTotalJobsSubmitted.fetch_add(1, std::memory_order_relaxed);

        {
            std::lock_guard lock(mGlobalMutex);
            mGlobalQueue.push(std::move(job));
        }

        mGlobalCondition.notify_one();
    }

    void JobSystem::SubmitToWorker(Job job, size_t workerId) {
        if (!mInitialized) {
            Log::Error("JobSystem", "Cannot submit job - system not initialized");
            return;
        }

        if (workerId >= mWorkers.size()) {
            Log::Warn("JobSystem", "Invalid worker ID {}, submitting to global queue", workerId);
            Submit(std::move(job));
            return;
        }

        mTotalJobsSubmitted.fetch_add(1, std::memory_order_relaxed);

        {
            std::lock_guard<std::mutex> lock(mWorkers[workerId].queueMutex);
            mWorkers[workerId].localQueue.push(std::move(job));
        }

        mGlobalCondition.notify_one();
    }

    shared_ptr<JobSystem::JobCounter> JobSystem::SubmitBatch(const vector<Job>& jobs) {
        if (jobs.empty()) {
            return nullptr;
        }

        auto counter       = make_shared<JobCounter>();
        counter->remaining = static_cast<i32>(jobs.size());

        for (const auto& job : jobs) {
            Submit([job, counter]() {
                job();
                counter->remaining.fetch_sub(1, std::memory_order_release);
            });
        }

        return counter;
    }

    shared_ptr<JobSystem::JobCounter> JobSystem::SubmitBatchChunked(const vector<Job>& jobs, size_t chunkSize) {
        if (jobs.empty()) {
            return nullptr;
        }

        // Auto-determine chunk size
        if (chunkSize == 0) {
            chunkSize = Math::Max(size_t(1), jobs.size() / (mWorkers.size() * 2));
            chunkSize = Math::Min(chunkSize, kDefaultChunkSize);
        }

        vector<Job> chunkedJobs;
        for (size_t i = 0; i < jobs.size(); i += chunkSize) {
            size_t end = Math::Min(i + chunkSize, jobs.size());

            // Create a job that executes a chunk
            chunkedJobs.push_back([&jobs, i, end]() {
                for (size_t j = i; j < end; ++j) {
                    jobs[j]();
                }
            });
        }

        return SubmitBatch(chunkedJobs);
    }

    void JobSystem::WaitForCounter(const shared_ptr<JobCounter>& counter) {
        if (!counter)
            return;

        // Help process jobs while waiting
        while (!counter->IsComplete()) {
            if (!ExecuteNextJob()) {
                // No jobs available, yield to reduce CPU usage
                std::this_thread::yield();
            }
        }
    }

    bool JobSystem::WaitForCounterTimeout(const shared_ptr<JobCounter>& counter, u32 timeoutMs) {
        if (!counter)
            return true;

        auto start   = std::chrono::steady_clock::now();
        auto timeout = std::chrono::milliseconds(timeoutMs);

        while (!counter->IsComplete()) {
            if (std::chrono::steady_clock::now() - start > timeout) {
                return false;  // Timeout
            }

            if (!ExecuteNextJob()) {
                std::this_thread::yield();
            }
        }

        return true;  // Completed
    }

    void JobSystem::HelpWith(const shared_ptr<JobCounter>& counter) {
        if (!counter)
            return;

        while (!counter->IsComplete()) {
            if (!ExecuteNextJob()) {
                break;  // No more jobs to help with
            }
        }
    }

    bool JobSystem::ExecuteNextJob() {
        if (!mInitialized)
            return false;

        Job job;
        i32 workerID = GetCurrentWorkerID();

        if (workerID >= 0) {
            // We're on a worker thread, try to get a job
            if (TryGetJob(static_cast<size_t>(workerID), job)) {
                job();
                mTotalJobsCompleted.fetch_add(1, std::memory_order_relaxed);
                return true;
            }
        } else {
            // We're on the main thread or external thread, try global queue
            std::lock_guard<std::mutex> lock(mGlobalMutex);
            if (!mGlobalQueue.empty()) {
                job = std::move(mGlobalQueue.front());
                mGlobalQueue.pop();

                // Execute without lock
                lock.~lock_guard();
                job();
                mTotalJobsCompleted.fetch_add(1, std::memory_order_relaxed);
                return true;
            }
        }

        return false;
    }

    i32 JobSystem::GetCurrentWorkerID() const {
        std::lock_guard<std::mutex> lock(mThreadMapMutex);
        auto it = mThreadIdToWorkerIndex.find(std::this_thread::get_id());
        return it != mThreadIdToWorkerIndex.end() ? static_cast<i32>(it->second) : -1;
    }

    JobSystem::Statistics JobSystem::GetStatistics() const {
        Statistics stats;
        stats.totalJobsSubmitted = mTotalJobsSubmitted.load(std::memory_order_relaxed);
        stats.totalJobsCompleted = mTotalJobsCompleted.load(std::memory_order_relaxed);

        {
            std::lock_guard<std::mutex> lock(mGlobalMutex);
            stats.jobsInGlobalQueue = mGlobalQueue.size();
        }

        stats.jobsPerWorker.resize(mWorkers.size());
        for (size_t i = 0; i < mWorkers.size(); ++i) {
            std::lock_guard<std::mutex> lock(mWorkers[i].queueMutex);
            stats.jobsPerWorker[i] = mWorkers[i].localQueue.size();
            stats.jobsInLocalQueues += stats.jobsPerWorker[i];
        }

        return stats;
    }

    void JobSystem::WorkerLoop(size_t workerId) {
        while (!mShutdown) {
            Job job;

            if (TryGetJob(workerId, job)) {
                // Execute the job
                job();
                mTotalJobsCompleted.fetch_add(1, std::memory_order_relaxed);
                mWorkers[workerId].jobsProcessed.fetch_add(1, std::memory_order_relaxed);
            } else {
                // No job found, wait for notification
                std::unique_lock<std::mutex> lock(mGlobalMutex);
                mGlobalCondition.wait_for(lock, std::chrono::milliseconds(10), [this]() {
                    return mShutdown.load() || !mGlobalQueue.empty();
                });
            }
        }
    }

    bool JobSystem::TryGetJob(size_t workerId, Job& outJob) {
        // 1. Try local queue first (best cache locality)
        {
            std::lock_guard<std::mutex> lock(mWorkers[workerId].queueMutex);
            if (!mWorkers[workerId].localQueue.empty()) {
                outJob = std::move(mWorkers[workerId].localQueue.front());
                mWorkers[workerId].localQueue.pop();
                return true;
            }
        }

        // 2. Try global queue
        {
            std::lock_guard<std::mutex> lock(mGlobalMutex);
            if (!mGlobalQueue.empty()) {
                outJob = std::move(mGlobalQueue.front());
                mGlobalQueue.pop();
                return true;
            }
        }

        // 3. Try work stealing from other workers
        return TryStealJob(workerId, outJob);
    }

    bool JobSystem::TryStealJob(size_t thiefId, Job& outJob) {
        // Random starting point to distribute stealing evenly
        size_t start = (thiefId + 1) % mWorkers.size();
        for (size_t i = 0; i < mWorkers.size(); ++i) {
            size_t victimId = (start + i) % mWorkers.size();
            if (victimId == thiefId)
                continue;

            std::lock_guard<std::mutex> lock(mWorkers[victimId].queueMutex);
            if (!mWorkers[victimId].localQueue.empty()) {
                outJob = std::move(mWorkers[victimId].localQueue.front());
                mWorkers[victimId].localQueue.pop();
                return true;
            }
        }

        return false;
    }
}  // namespace Astera