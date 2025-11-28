// Author: Jake Rieger
// Created: 11/27/25.
//

#pragma once

#include <print>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/fmt.h>

// For some reason, this has to be a relative path like so
#include "../Vendor/glad/glad.h"

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

#define CAST static_cast
#define CCAST const_cast
#define DCAST dynamic_cast
#define RCAST reinterpret_cast

namespace N {
    using u8   = uint8_t;
    using u16  = uint16_t;
    using u32  = uint32_t;
    using u64  = uint64_t;
    using uptr = uintptr_t;

    using s8   = int8_t;
    using s16  = int16_t;
    using s32  = int32_t;
    using s64  = int64_t;
    using sptr = intptr_t;

#if defined(__GNUC__) || defined(__clang__)
    using u128 = __uint128_t;
    using s128 = __int128_t;
#endif

    using f32 = float;
    using f64 = double;

    using std::array;
    using std::make_shared;
    using std::make_unique;
    using std::optional;
    using std::print;
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

    /// @brief Simple exception container for breaking in functions that haven't been implemented yet
    class NotImplemented final : public std::exception {
    public:
        explicit NotImplemented(const char* funcName, const char* fileName, s32 line) {
            mMessage = std::format("\n`{}` is not implemented in {} ({})\n", funcName, fileName, line);
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
}  // namespace N
