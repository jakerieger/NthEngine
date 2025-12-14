/*
 *  Filename: JobSystem.hpp
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

#pragma once

#include "EngineCommon.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <functional>

namespace Astera {
    /// @brief High-performance work-stealing job system for parallel task execution
    ///
    /// Provides a thread pool with work stealing for efficient load balancing.
    /// Jobs can be submitted individually or in batches, with synchronization
    /// primitives for waiting on completion.
    class JobSystem {
    public:
        /// @brief Function signature for jobs
        using Job = std::function<void()>;

        /// @brief Counter for tracking batch job completion
        struct JobCounter {
            std::atomic<i32> remaining {0};

            /// @brief Check if all jobs are complete
            ASTERA_KEEP bool IsComplete() const {
                return remaining.load(std::memory_order_acquire) <= 0;
            }
        };

        JobSystem() = default;
        ~JobSystem();

        ASTERA_CLASS_PREVENT_MOVES_COPIES(JobSystem)

        /// @brief Initialize the job system with worker threads
        /// @param numThreads Number of worker threads (default: hardware concurrency)
        void Initialize(size_t numThreads = 0);

        /// @brief Shutdown the job system and wait for all workers to finish
        void Shutdown();

        /// @brief Submit a single job for execution
        /// @param job The job function to execute
        void Submit(Job job);

        /// @brief Submit a job to a specific worker's local queue
        /// @param job The job function to execute
        /// @param workerId The ID of the worker to submit to
        void SubmitToWorker(Job job, size_t workerId);

        /// @brief Submit multiple jobs and get a counter to track completion
        /// @param jobs Vector of jobs to execute
        /// @return Shared counter for tracking batch completion
        shared_ptr<JobCounter> SubmitBatch(const vector<Job>& jobs);

        /// @brief Submit multiple jobs with automatic chunking
        /// @param jobs Vector of jobs to execute
        /// @param chunkSize Number of jobs to group together (0 = auto)
        /// @return Shared counter for tracking batch completion
        shared_ptr<JobCounter> SubmitBatchChunked(const vector<Job>& jobs, size_t chunkSize = 0);

        /// @brief Wait for a job counter to reach zero (blocking)
        /// @param counter The counter to wait on
        void WaitForCounter(const shared_ptr<JobCounter>& counter);

        /// @brief Wait for a job counter with timeout
        /// @param counter The counter to wait on
        /// @param timeoutMs Timeout in milliseconds
        /// @return True if completed, false if timed out
        bool WaitForCounterTimeout(const shared_ptr<JobCounter>& counter, u32 timeoutMs);

        /// @brief Process jobs on the calling thread until counter is zero
        /// @param counter The counter to help complete
        void HelpWith(const shared_ptr<JobCounter>& counter);

        /// @brief Execute a single pending job on the calling thread
        /// @return True if a job was executed, false if no jobs available
        bool ExecuteNextJob();

        /// @brief Get the number of worker threads
        ASTERA_KEEP size_t GetWorkerCount() const {
            return mWorkers.size();
        }

        /// @brief Get the current thread's worker ID (if it's a worker thread)
        /// @return Worker ID, or -1 if not a worker thread
        ASTERA_KEEP i32 GetCurrentWorkerID() const;

        /// @brief Check if the job system is initialized
        ASTERA_KEEP bool IsInitialized() const {
            return mInitialized;
        }

        /// @brief Get statistics about the job system
        struct Statistics {
            size_t totalJobsSubmitted {0};
            size_t totalJobsCompleted {0};
            size_t jobsInGlobalQueue {0};
            size_t jobsInLocalQueues {0};
            vector<size_t> jobsPerWorker;
        };

        Statistics GetStatistics() const;

    private:
        /// @brief Internal worker thread state
        struct WorkerThread {
            std::thread thread;  // This is fine - std::thread has a default constructor
            std::queue<Job> localQueue;
            mutable std::mutex queueMutex;
            std::atomic<size_t> jobsProcessed {0};
            std::thread::id threadId;

            // Default constructor
            WorkerThread() = default;

            // Move constructor and assignment (needed for vector operations)
            WorkerThread(WorkerThread&& other) noexcept
                : thread(std::move(other.thread)), localQueue(std::move(other.localQueue)),
                  jobsProcessed(other.jobsProcessed.load()), threadId(other.threadId) {}

            WorkerThread& operator=(WorkerThread&& other) noexcept {
                if (this != &other) {
                    thread     = std::move(other.thread);
                    localQueue = std::move(other.localQueue);
                    jobsProcessed.store(other.jobsProcessed.load());
                    threadId = other.threadId;
                }
                return *this;
            }

            // Delete copy operations (threads can't be copied)
            WorkerThread(const WorkerThread&)            = delete;
            WorkerThread& operator=(const WorkerThread&) = delete;
        };

        /// @brief Worker thread main loop
        /// @param workerId ID of this worker
        void WorkerLoop(size_t workerId);

        /// @brief Try to get a job from any available source
        /// @param workerId ID of the calling worker
        /// @param outJob Output parameter for the job
        /// @return True if a job was obtained
        bool TryGetJob(size_t workerId, Job& outJob);

        /// @brief Try to steal a job from another worker
        /// @param thiefId ID of the worker trying to steal
        /// @param outJob Output parameter for the stolen job
        /// @return True if a job was stolen
        bool TryStealJob(size_t thiefId, Job& outJob);

        /// @brief Worker threads
        vector<WorkerThread> mWorkers;

        /// @brief Global job queue (fallback when local queues are empty)
        std::queue<Job> mGlobalQueue;
        mutable std::mutex mGlobalMutex;
        std::condition_variable mGlobalCondition;

        /// @brief Shutdown flag
        std::atomic<bool> mShutdown {false};

        /// @brief Initialization flag
        std::atomic<bool> mInitialized {false};

        /// @brief Statistics tracking
        std::atomic<size_t> mTotalJobsSubmitted {0};
        std::atomic<size_t> mTotalJobsCompleted {0};

        /// @brief Map thread IDs to worker indices
        std::unordered_map<std::thread::id, size_t> mThreadIdToWorkerIndex;
        mutable std::mutex mThreadMapMutex;

        /// @brief Default chunk size for batch submission
        static constexpr size_t kDefaultChunkSize = 64;
    };

    /// @brief Global job system instance
    extern unique_ptr<JobSystem> gJobSystem;

    /// @brief Helper for parallel-for loops
    /// @tparam Func Function signature: void(size_t index)
    /// @param start Start index (inclusive)
    /// @param end End index (exclusive)
    /// @param func Function to execute for each index
    /// @param chunkSize Number of iterations per job (0 = auto)
    template<typename Func>
    void ParallelFor(size_t start, size_t end, Func func, size_t chunkSize = 0) {
        if (!gJobSystem || !gJobSystem->IsInitialized()) {
            // Fallback to serial execution
            for (size_t i = start; i < end; ++i) {
                func(i);
            }
            return;
        }

        const size_t count = end - start;
        if (count == 0)
            return;

        // Auto-determine chunk size
        if (chunkSize == 0) {
            const size_t numWorkers = gJobSystem->GetWorkerCount();
            chunkSize               = std::max(size_t(1), count / (numWorkers * 4));
        }

        vector<JobSystem::Job> jobs;
        for (size_t i = start; i < end; i += chunkSize) {
            size_t chunkEnd = std::min(i + chunkSize, end);
            jobs.push_back([i, chunkEnd, &func]() {
                for (size_t j = i; j < chunkEnd; ++j) {
                    func(j);
                }
            });
        }

        auto counter = gJobSystem->SubmitBatch(jobs);
        gJobSystem->WaitForCounter(counter);
    }

    /// @brief Helper for parallel-for loops with worker ID awareness
    /// @tparam Func Function signature: void(size_t index, size_t workerID)
    template<typename Func>
    void ParallelForIndexed(size_t start, size_t end, Func func, size_t chunkSize = 0) {
        if (!gJobSystem || !gJobSystem->IsInitialized()) {
            for (size_t i = start; i < end; ++i) {
                func(i, 0);
            }
            return;
        }

        const size_t count = end - start;
        if (count == 0)
            return;

        if (chunkSize == 0) {
            const size_t numWorkers = gJobSystem->GetWorkerCount();
            chunkSize               = std::max(size_t(1), count / (numWorkers * 4));
        }

        vector<JobSystem::Job> jobs;
        for (size_t i = start; i < end; i += chunkSize) {
            size_t chunkEnd = std::min(i + chunkSize, end);
            jobs.push_back([i, chunkEnd, &func]() {
                i32 workerID       = gJobSystem->GetCurrentWorkerID();
                size_t workerIndex = workerID >= 0 ? static_cast<size_t>(workerID) : 0;
                for (size_t j = i; j < chunkEnd; ++j) {
                    func(j, workerIndex);
                }
            });
        }

        auto counter = gJobSystem->SubmitBatch(jobs);
        gJobSystem->WaitForCounter(counter);
    }
}  // namespace Astera