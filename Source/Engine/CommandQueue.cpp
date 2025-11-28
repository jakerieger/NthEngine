// Author: Jake Rieger
// Created: 11/27/25.
//

#include "CommandQueue.hpp"
#include "Log.hpp"

namespace N {
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

        glClearColor(cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);

        if (cmd.clearDepth) { clearFlags |= GL_DEPTH_BUFFER_BIT; }

        if (cmd.clearStencil) { clearFlags |= GL_STENCIL_BUFFER_BIT; }

        glClear(clearFlags);
    }

    void CommandExecutor::operator()(const DrawSpriteCommand& cmd) const {
        // TODO: Implement sprite rendering
        N_UNUSED(cmd);
        throw N_NOT_IMPLEMENTED;
    }

    void CommandExecutor::operator()(const SetViewportCommand& cmd) const {
        glViewport(cmd.x, cmd.y, CAST<GLsizei>(cmd.width), CAST<GLsizei>(cmd.height));
    }

    void CommandExecutor::operator()(const BindShaderCommand& cmd) const {
        glUseProgram(cmd.programId);
    }

    void CommandExecutor::operator()(const SetUniformCommand& cmd) const {
        glUseProgram(cmd.programId);

        const GLint location = glGetUniformLocation(cmd.programId, cmd.name.c_str());
        if (location == -1) {
            Log::Warn("Uniform '{}' not found in shader program {}", cmd.name, cmd.programId);
            return;
        }

        // Use std::visit to handle the different uniform value types
        std::visit(
          [location]<typename T0>(const T0& value) {
              using T = std::decay_t<T0>;

              if constexpr (std::is_same_v<T, s32>) {
                  glUniform1i(location, value);
              } else if constexpr (std::is_same_v<T, f32>) {
                  glUniform1f(location, value);
              } else if constexpr (std::is_same_v<T, glm::vec2>) {
                  glUniform2f(location, value.x, value.y);
              } else if constexpr (std::is_same_v<T, glm::vec3>) {
                  glUniform3f(location, value.x, value.y, value.z);
              } else if constexpr (std::is_same_v<T, glm::vec4>) {
                  glUniform4f(location, value.x, value.y, value.z, value.w);
              } else if constexpr (std::is_same_v<T, glm::mat4>) {
                  glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
              }
          },
          cmd.value);
    }
}  // namespace N