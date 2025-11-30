// Author: Jake Rieger
// Created: 11/27/25.
//

#pragma once

#include <variant>

#include "CommonPCH.hpp"
#include "Components/Transform.hpp"

namespace Nth {
    /// @brief Command to clear the framebuffer
    struct ClearCommand {
        Vec4 color {0.0f, 0.0f, 0.0f, 1.0f};
        bool clearDepth {true};
        bool clearStencil {false};
    };

    /// @brief Command to draw a sprite/quad
    struct DrawSpriteCommand {
        u32 textureId {0};
        Transform transform;
        Vec2 screenDimensions;
        Vec4 tintColor {1.0f, 1.0f, 1.0f, 1.0f};
    };

    /// @brief Command to set the viewport
    struct SetViewportCommand {
        i32 x {0};
        i32 y {0};
        u32 width {800};
        u32 height {600};
    };

    /// @brief Command to bind a shader program
    struct BindShaderCommand {
        u32 programId {0};
    };

    /// @brief Command to set a uniform value
    struct SetUniformCommand {
        u32 programId {0};
        string name;
        std::variant<i32, f32, Vec2, Vec3, Vec4, Mat4> value;
    };

    /// @brief Variant type that can hold any command
    using RenderCommand =
      std::variant<ClearCommand, DrawSpriteCommand, SetViewportCommand, BindShaderCommand, SetUniformCommand>;
}  // namespace Nth
