/*
 *  Filename: ArenaAllocator.cpp
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

#include "ArenaAllocator.hpp"

namespace Astera {
    ArenaAllocator::ArenaAllocator(size_t size) : mTotalSize(size) {
        mMemory     = new u8[size];
        mCurrentPos = mMemory;
    }

    ArenaAllocator::~ArenaAllocator() {
        delete[] mMemory;
    }

    ArenaAllocator::ArenaAllocator(ArenaAllocator&& other) noexcept
        : mMemory(other.mMemory), mCurrentPos(other.mCurrentPos), mTotalSize(other.mTotalSize) {
        other.mMemory     = nullptr;
        other.mCurrentPos = nullptr;
        other.mTotalSize  = 0;
    }

    ArenaAllocator& ArenaAllocator::operator=(ArenaAllocator&& other) noexcept {
        if (this != &other) {
            delete[] mMemory;

            mMemory     = other.mMemory;
            mCurrentPos = other.mCurrentPos;
            mTotalSize  = other.mTotalSize;

            other.mMemory     = nullptr;
            other.mCurrentPos = nullptr;
            other.mTotalSize  = 0;
        }

        return *this;
    }

    void* ArenaAllocator::Allocate(size_t size, size_t alignment) {
        const auto currentAddr = RCAST<size_t>(mCurrentPos);
        const auto alignAddr   = ASTERA_ALIGN_UP(currentAddr, alignment);
        const auto adjustment  = alignAddr - currentAddr;

        if (mCurrentPos + size + adjustment > mMemory + mTotalSize) {
            return nullptr;  // out of memory :(
        }

        mCurrentPos = RCAST<u8*>(alignAddr + size);
        return RCAST<void*>(alignAddr);
    }

    void ArenaAllocator::Reset() {
        mCurrentPos = mMemory;
    }

    size_t ArenaAllocator::GetUsedMemory() const {
        return CAST<size_t>(mCurrentPos - mMemory);
    }

    size_t ArenaAllocator::GetSize() const {
        return mTotalSize;
    }

    size_t ArenaAllocator::GetAvailableMemory() const {
        return mTotalSize - GetUsedMemory();
    }
}  // namespace Astera