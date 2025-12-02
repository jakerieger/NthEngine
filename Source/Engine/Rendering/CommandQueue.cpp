/// @author Jake Rieger
/// @created 11/27/25
///
#include "CommandQueue.hpp"

#include "Coordinates.hpp"
#include "Geometry.hpp"
#include "Log.hpp"
#include "ShaderManager.hpp"
#include "Rendering/GLUtils.hpp"

namespace Nth {
    void CommandQueue::ExecuteQueue() {
        CommandExecutor executor;

        for (const auto& command : mCommands) {
            std::visit(executor, command);
        }

        Clear();
    }

    void CommandQueue::Clear() {
        mCommands.clear();
    }

    void CommandQueue::ExecuteCommand(const RenderCommand& command) {
        CommandExecutor executor;
        std::visit(executor, command);
    }

    // ============================================================================
    // CommandExecutor Implementation
    // ============================================================================

    void CommandExecutor::operator()(const ClearCommand& cmd) const {
        GLbitfield clearFlags = GL_COLOR_BUFFER_BIT;

        GLCall(glClearColor, cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);

        if (cmd.clearDepth) { clearFlags |= GL_DEPTH_BUFFER_BIT; }

        if (cmd.clearStencil) { clearFlags |= GL_STENCIL_BUFFER_BIT; }

        GLCall(glClear, clearFlags);
    }

    void CommandExecutor::operator()(const DrawSpriteCommand& cmd) const {
        auto* spriteShader = ShaderManager::GetShader(Shaders::Sprite);
        N_ASSERT(spriteShader);
        spriteShader->Bind();

        // Bind the sprite texture
        GLCall(glActiveTexture, GL_TEXTURE0);
        GLCall(glBindTexture, GL_TEXTURE_2D, cmd.sprite.textureId);
        spriteShader->SetUniform("uSprite", 0);

        // Calculate MVP matrix
        const Mat4 model      = cmd.transform.GetMatrix();
        const Mat4 projection = Coordinates::CreateScreenProjection(cmd.screenDimensions.x, cmd.screenDimensions.y);
        const Mat4 mvp        = projection * model;
        spriteShader->SetUniform("uMVP", mvp);

        cmd.sprite.geometry->DrawIndexed();
        spriteShader->Unbind();
    }

    void CommandExecutor::operator()(const SetViewportCommand& cmd) const {
        GLCall(glViewport, cmd.x, cmd.y, CAST<GLsizei>(cmd.width), CAST<GLsizei>(cmd.height));
    }

    void CommandExecutor::operator()(const BindShaderCommand& cmd) const {
        GLCall(glUseProgram, cmd.programId);
    }

    void CommandExecutor::operator()(const SetUniformCommand& cmd) const {
        GLCall(glUseProgram, cmd.programId);

        const GLint location = glGetUniformLocation(cmd.programId, cmd.name.c_str());
        if (location == -1) {
            Log::Warn("CommandQueue", "Uniform '{}' not found in shader program {}", cmd.name, cmd.programId);
            return;
        }

        // Use std::visit to handle the different uniform value types
        std::visit(
          [location]<typename T0>(const T0& value) {
              using T = std::decay_t<T0>;

              if constexpr (std::is_same_v<T, i32>) {
                  GLCall(glUniform1i, location, value);
              } else if constexpr (std::is_same_v<T, f32>) {
                  GLCall(glUniform1f, location, value);
              } else if constexpr (std::is_same_v<T, glm::vec2>) {
                  GLCall(glUniform2f, location, value.x, value.y);
              } else if constexpr (std::is_same_v<T, glm::vec3>) {
                  GLCall(glUniform3f, location, value.x, value.y, value.z);
              } else if constexpr (std::is_same_v<T, glm::vec4>) {
                  GLCall(glUniform4f, location, value.x, value.y, value.z, value.w);
              } else if constexpr (std::is_same_v<T, glm::mat4>) {
                  GLCall(glUniformMatrix4fv, location, 1, GL_FALSE, &value[0][0]);
              }
          },
          cmd.value);
    }
}  // namespace Nth