/*
 *  Filename: FrameAllocator.hpp
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

#pragma once

#include "EngineCommon.hpp"

namespace Astera {
    /// @brief A linear allocator optimized for per-frame allocations. Memory is automatically reset
    /// at the beginning of each frame. Supports double-buffering to allow previous frame data to
    /// remain valid during the current frame.
    class FrameAllocator {
    public:
        /// @brief Creates a frame allocator with the specified size per buffer
        /// @param sizePerBuffer Size of each buffer in bytes
        /// @param bufferCount Number of buffers (1 for single-buffered, 2 for double-buffered, etc.)
        explicit FrameAllocator(size_t sizePerBuffer, u32 bufferCount = 2);
        ~FrameAllocator();

        FrameAllocator(FrameAllocator&& other) noexcept;
        FrameAllocator& operator=(FrameAllocator&& other) noexcept;

        ASTERA_CLASS_PREVENT_COPIES(FrameAllocator)

        /// @brief Allocates memory from the current frame buffer
        /// @param size Size in bytes to allocate
        /// @param alignment Alignment requirement (must be power of 2)
        /// @return Pointer to allocated memory, or nullptr if allocation failed
        void* Allocate(size_t size, size_t alignment = kMinAlignment);

        /// @brief Allocates memory for the specified type and count
        /// @tparam T Type to allocate
        /// @param count Number of elements to allocate
        /// @return Typed pointer to allocated memory
        template<typename T>
        T* AllocateType(size_t count = 1) {
            return CAST<T*>(Allocate(sizeof(T) * count, alignof(T)));
        }

        /// @brief Advances to the next frame, resetting the previous current buffer
        /// Call this at the beginning of each frame
        void NextFrame();

        /// @brief Manually reset the current frame buffer (typically not needed if using NextFrame)
        void Reset() const;

        /// @brief Resets all buffers immediately
        void ResetAll() const;

        /// @brief Returns memory in use by the current frame buffer in bytes
        ASTERA_KEEP size_t GetUsedMemory() const;

        /// @brief Returns memory in use across all buffers in bytes
        ASTERA_KEEP size_t GetTotalUsedMemory() const;

        /// @brief Returns the size of a single buffer in bytes
        ASTERA_KEEP size_t GetBufferSize() const;

        /// @brief Returns available memory in the current frame buffer in bytes
        ASTERA_KEEP size_t GetAvailableMemory() const;

        /// @brief Returns the number of buffers
        ASTERA_KEEP u32 GetBufferCount() const;

        /// @brief Returns the current frame index (buffer being used)
        ASTERA_KEEP u32 GetCurrentFrame() const;

        /// @brief Returns the peak memory usage for the current buffer since last reset
        ASTERA_KEEP size_t GetPeakMemoryUsage() const;

    private:
        struct FrameBuffer {
            u8* memory;
            u8* currentPos;
            size_t size;
            size_t peakUsage;
        };

        FrameBuffer* mBuffers;
        u32 mBufferCount;
        u32 mCurrentBuffer;
        static constexpr size_t kMinAlignment = alignof(std::max_align_t);
    };
}  // namespace Astera