// /*
//  *  Filename: ${NAME}
//  *  This code is part of the Astera core library
//  *  Copyright ${YEAR} Jake Rieger
//  *
//  *  Licensed under the Apache License, Version 2.0 (the "License");
//  *  you may not use this file except in compliance with the License.
//  *  You may obtain a copy of the License at
//  *
//  *      http://www.apache.org/licenses/LICENSE-2.0
//  *
//  *  Unless required by applicable law or agreed to in writing, software
//  *  distributed under the License is distributed on an "AS IS" BASIS,
//  *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  *  See the License for the specific language governing permissions and
//  *  limitations under the License.
//  *
//  *  In no event and under no legal theory, whether in tort (including negligence),
//  *  contract, or otherwise, unless required by applicable law (such as deliberate
//  *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
//  *  liable for any damages, including any direct, indirect, special, incidental,
//  *  or consequential damages of any character arising as a result of this License or
//  *  out of the use or inability to use the software (including but not limited to damages
//  *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
//  *  all other commercial damages or losses), even if such Contributor has been advised
//  *  of the possibility of such damages.
//  *

/// @author Jake Rieger
/// @created 11/27/25
///

#pragma once

#include <charconv>
#include <exception>
#include <filesystem>
#include <iterator>
#include <optional>
#include <ranges>
#include <ranges>
#include <source_location>
#include <string_view>
#include <system_error>
#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <fmt/format.h>

#include <glad.h>
#include <GLFW/glfw3.h>

#if defined(ASTERA_PLATFORM_WINDOWS)
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(ASTERA_PLATFORM_LINUX)
    #define GLFW_EXPOSE_NATIVE_X11
#elif defined(ASTERA_PLATFORM_MACOS)
    #define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

#include "Common/Macros.hpp"
#include "Common/IO.hpp"
#include "Memory.hpp"

#define CAST static_cast
#define CCAST const_cast
#define DCAST dynamic_cast
#define RCAST reinterpret_cast

/// @brief Root namespace containing all Astera code
namespace Astera {
    using u8   = uint8_t;
    using u16  = uint16_t;
    using u32  = uint32_t;
    using u64  = uint64_t;
    using uptr = uintptr_t;

    using i8   = int8_t;
    using i16  = int16_t;
    using i32  = int32_t;
    using i64  = int64_t;
    using iptr = intptr_t;

#ifndef _MSC_VER
    using u128 = __uint128_t;
    using i128 = __int128_t;
#endif

    using f32 = float;
    using f64 = double;

    using std::array;
    using std::make_shared;
    using std::make_unique;
    using std::optional;
    using std::shared_ptr;
    using std::string;
    using std::unique_ptr;
    using std::unordered_map;
    using std::vector;
    using std::weak_ptr;

    using Vec2 = glm::vec2;
    using Vec3 = glm::vec3;
    using Vec4 = glm::vec4;
    using Mat3 = glm::mat3;
    using Mat4 = glm::mat4;
    using Quat = glm::quat;

    namespace fs = std::filesystem;

    // Forward declarations
    class Game;

    /// @brief Simple exception container for breaking in functions that haven't been implemented yet
    class NotImplemented final : public std::exception {
    public:
        explicit NotImplemented(const char* funcName, const char* fileName, i32 line) {
            mMessage = fmt::format("\n`{}` is not implemented in {} ({})\n", funcName, fileName, line);
        }

        [[nodiscard]] const char* what() const noexcept override {
            return mMessage.c_str();
        }

    private:
        string mMessage;
    };

#if defined(__GNUC__) || defined(__clang__)
    #define ASTERA_NOT_IMPLEMENTED NotImplemented(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#elif defined(_MSC_VER)
    #define ASTERA_NOT_IMPLEMENTED NotImplemented(__FUNCSIG__, __FILE__, __LINE__)
#else
    #define ASTERA_NOT_IMPLEMENTED NotImplemented(__func__, __FILE__, __LINE__)
#endif

    /// @brief Returns the size of an iterable
    template<std::ranges::view Iterable>
    auto Distance(const Iterable& container) {
        return std::ranges::distance(container);
    }

    inline f32 StringToF32(const string& str) {
        f32 result;
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);

        if (ec == std::errc()) {
            // Success - also check if the entire string was consumed
            if (ptr == str.data() + str.size()) {
                return result;
            } else {
                // Partial parse: trailing characters exist
                throw std::invalid_argument("Trailing characters in string");
            }
        } else if (ec == std::errc::invalid_argument) {
            throw std::invalid_argument("Invalid float format");
        } else if (ec == std::errc::result_out_of_range) {
            throw std::range_error("Float value out of range");
        }

        throw std::runtime_error("Unknown conversion error");
    }
}  // namespace Astera