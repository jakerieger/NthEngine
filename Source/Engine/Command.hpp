// Author: Jake Rieger
// Created: 11/27/25.
//

#pragma once

#include "CommonPCH.hpp"

namespace N {
    /// @brief Command to clear the framebuffer
    struct ClearCommand {
        glm::vec4 color {0.0f, 0.0f, 0.0f, 1.0f};
        bool clearDepth {true};
        bool clearStencil {false};
    };

    /// @brief Command to draw a sprite/quad
    struct DrawSpriteCommand {
        u32 textureId {0};
        glm::vec2 position {0.0f, 0.0f};
        glm::vec2 size {1.0f, 1.0f};
        glm::vec4 tintColor {1.0f, 1.0f, 1.0f, 1.0f};
        f32 rotation {0.0f};
    };

    /// @brief Command to set the viewport
    struct SetViewportCommand {
        s32 x {0};
        s32 y {0};
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
        std::variant<s32, f32, glm::vec2, glm::vec3, glm::vec4, glm::mat4> value;
    };

    /// @brief Variant type that can hold any command
    using RenderCommand =
      std::variant<ClearCommand, DrawSpriteCommand, SetViewportCommand, BindShaderCommand, SetUniformCommand>;
}  // namespace N
