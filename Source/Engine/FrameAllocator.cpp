/*
 *  Filename: FrameAllocator.cpp
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
 *
 *  In no event and under no legal theory, whether in tort (including negligence),
 *  contract, or otherwise, unless required by applicable law (such as deliberate
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental,
 *  or consequential damages of any character arising as a result of this License or
 *  out of the use or inability to use the software (including but not limited to damages
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
 *  all other commercial damages or losses), even if such Contributor has been advised
 *  of the possibility of such damages.
 */

#include "FrameAllocator.hpp"

namespace Astera {
    FrameAllocator::FrameAllocator(size_t sizePerBuffer, u32 bufferCount)
        : mBufferCount(bufferCount), mCurrentBuffer(0) {
        mBuffers = new FrameBuffer[bufferCount];

        for (u32 i = 0; i < bufferCount; ++i) {
            mBuffers[i].memory     = new u8[sizePerBuffer];
            mBuffers[i].currentPos = mBuffers[i].memory;
            mBuffers[i].size       = sizePerBuffer;
            mBuffers[i].peakUsage  = 0;
        }
    }

    FrameAllocator::~FrameAllocator() {
        if (mBuffers) {
            for (u32 i = 0; i < mBufferCount; ++i) {
                delete[] mBuffers[i].memory;
            }
            delete[] mBuffers;
        }
    }

    FrameAllocator::FrameAllocator(FrameAllocator&& other) noexcept
        : mBuffers(other.mBuffers), mBufferCount(other.mBufferCount), mCurrentBuffer(other.mCurrentBuffer) {
        other.mBuffers       = nullptr;
        other.mBufferCount   = 0;
        other.mCurrentBuffer = 0;
    }

    FrameAllocator& FrameAllocator::operator=(FrameAllocator&& other) noexcept {
        if (this != &other) {
            // Clean up existing resources
            if (mBuffers) {
                for (u32 i = 0; i < mBufferCount; ++i) {
                    delete[] mBuffers[i].memory;
                }
                delete[] mBuffers;
            }

            // Transfer ownership
            mBuffers       = other.mBuffers;
            mBufferCount   = other.mBufferCount;
            mCurrentBuffer = other.mCurrentBuffer;

            // Reset other
            other.mBuffers       = nullptr;
            other.mBufferCount   = 0;
            other.mCurrentBuffer = 0;
        }

        return *this;
    }

    void* FrameAllocator::Allocate(size_t size, size_t alignment) {
        FrameBuffer& buffer = mBuffers[mCurrentBuffer];

        const auto currentAddr = RCAST<size_t>(buffer.currentPos);
        const auto alignAddr   = ASTERA_ALIGN_UP(currentAddr, alignment);
        const auto adjustment  = alignAddr - currentAddr;

        // Check if we have enough space
        if (buffer.currentPos + size + adjustment > buffer.memory + buffer.size) {
            return nullptr;  // Out of memory in the current frame buffer
        }

        // Update position
        buffer.currentPos = RCAST<u8*>(alignAddr + size);

        // Track peak usage
        const size_t currentUsage = CAST<size_t>(buffer.currentPos - buffer.memory);
        if (currentUsage > buffer.peakUsage) { buffer.peakUsage = currentUsage; }

        return RCAST<void*>(alignAddr);
    }

    void FrameAllocator::NextFrame() {
        // Move to the next buffer
        mCurrentBuffer = (mCurrentBuffer + 1) % mBufferCount;

        // Reset the new current buffer (it was used N frames ago)
        FrameBuffer& buffer = mBuffers[mCurrentBuffer];
        buffer.currentPos   = buffer.memory;
        buffer.peakUsage    = 0;
    }

    void FrameAllocator::Reset() const {
        FrameBuffer& buffer = mBuffers[mCurrentBuffer];
        buffer.currentPos   = buffer.memory;
        buffer.peakUsage    = 0;
    }

    void FrameAllocator::ResetAll() const {
        for (u32 i = 0; i < mBufferCount; ++i) {
            mBuffers[i].currentPos = mBuffers[i].memory;
            mBuffers[i].peakUsage  = 0;
        }
    }

    size_t FrameAllocator::GetUsedMemory() const {
        const FrameBuffer& buffer = mBuffers[mCurrentBuffer];
        return CAST<size_t>(buffer.currentPos - buffer.memory);
    }

    size_t FrameAllocator::GetTotalUsedMemory() const {
        size_t total = 0;
        for (u32 i = 0; i < mBufferCount; ++i) {
            total += CAST<size_t>(mBuffers[i].currentPos - mBuffers[i].memory);
        }
        return total;
    }

    size_t FrameAllocator::GetBufferSize() const {
        return mBuffers[0].size;
    }

    size_t FrameAllocator::GetAvailableMemory() const {
        const FrameBuffer& buffer = mBuffers[mCurrentBuffer];
        return buffer.size - CAST<size_t>(buffer.currentPos - buffer.memory);
    }

    u32 FrameAllocator::GetBufferCount() const {
        return mBufferCount;
    }

    u32 FrameAllocator::GetCurrentFrame() const {
        return mCurrentBuffer;
    }

    size_t FrameAllocator::GetPeakMemoryUsage() const {
        return mBuffers[mCurrentBuffer].peakUsage;
    }
}  // namespace Astera