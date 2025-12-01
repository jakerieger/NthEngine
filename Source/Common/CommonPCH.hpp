// Author: Jake Rieger
// Created: 11/27/25.
//

#pragma once

#include <iterator>
#include <ranges>
#include <filesystem>
#include <utility>
#include <string_view>
#include <source_location>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define SPDLOG_NO_FMT_STRING
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <fmt/format.h>

// For some reason, this has to be a relative path like so
#include "../Vendor/glad.h"

#include <GLFW/glfw3.h>

#if defined(N_ENGINE_PLATFORM_WINDOWS)
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(N_ENGINE_PLATFORM_LINUX_X11)
    #define GLFW_EXPOSE_NATIVE_X11
#elif defined(N_ENGINE_PLATFORM_LINUX_WAYLAND)
    #define GLFW_EXPOSE_NATIVE_WAYLAND
#elif defined(N_ENGINE_PLATFORM_MACOS)
    #define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

#include "Macros.hpp"
#include "IO.hpp"

#define CAST static_cast
#define CCAST const_cast
#define DCAST dynamic_cast
#define RCAST reinterpret_cast

/// @brief Root namespace containing all Nth Engine code
namespace Nth {
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
    #define N_NOT_IMPLEMENTED NotImplemented(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#elif defined(_MSC_VER)
    #define N_NOT_IMPLEMENTED NotImplemented(__FUNCSIG__, __FILE__, __LINE__)
#else
    #define N_NOT_IMPLEMENTED NotImplemented(__func__, __FILE__, __LINE__)
#endif

    /// @brief Returns the size of an iterable
    template<std::ranges::view Iterable>
    auto Distance(const Iterable& container) {
        return std::ranges::distance(container);
    }
}  // namespace Nth