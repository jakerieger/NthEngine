/*
 *  Filename: Memory.hpp
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

#include <memory>

namespace Astera {
    // Bytes
    constexpr size_t operator""_B(unsigned long long value) {
        return static_cast<size_t>(value);
    }

    // Kilobytes (1024 bytes)
    constexpr size_t operator""_KB(unsigned long long value) {
        return static_cast<size_t>(value * 1024ULL);
    }

    // Megabytes (1024 * 1024 bytes)
    constexpr size_t operator""_MB(unsigned long long value) {
        return static_cast<size_t>(value * 1024ULL * 1024ULL);
    }

    // Gigabytes (1024 * 1024 * 1024 bytes)
    constexpr size_t operator""_GB(unsigned long long value) {
        return static_cast<size_t>(value * 1024ULL * 1024ULL * 1024ULL);
    }

    // Terabytes (1024^4 bytes)
    constexpr size_t operator""_TB(unsigned long long value) {
        return static_cast<size_t>(value * 1024ULL * 1024ULL * 1024ULL * 1024ULL);
    }

    // Decimal versions (base 10) - useful for matching hardware specs
    constexpr size_t operator""_kB(unsigned long long value) {
        return static_cast<size_t>(value * 1000ULL);
    }

    constexpr size_t operator""_mB(unsigned long long value) {
        return static_cast<size_t>(value * 1000ULL * 1000ULL);
    }

    constexpr size_t operator""_gB(unsigned long long value) {
        return static_cast<size_t>(value * 1000ULL * 1000ULL * 1000ULL);
    }

    constexpr size_t operator""_tB(unsigned long long value) {
        return static_cast<size_t>(value * 1000ULL * 1000ULL * 1000ULL * 1000ULL);
    }

    constexpr size_t BytesToKB(size_t bytes) {
        return bytes / 1024;
    }

    constexpr size_t BytesToMB(size_t bytes) {
        return bytes / (1024 * 1024);
    }

    constexpr size_t BytesToGB(size_t bytes) {
        return bytes / (1024 * 1024 * 1024);
    }

    constexpr double BytesToKBPrecise(size_t bytes) {
        return static_cast<double>(bytes) / 1024.0;
    }

    constexpr double BytesToMBPrecise(size_t bytes) {
        return static_cast<double>(bytes) / (1024.0 * 1024.0);
    }

    constexpr double BytesToGBPrecise(size_t bytes) {
        return static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0);
    }
}  // namespace Astera