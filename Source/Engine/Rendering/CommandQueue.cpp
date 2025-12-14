/*
 *  Filename: CommandQueue.cpp
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

#include "CommandQueue.hpp"

#include "Coordinates.inl"
#include "Geometry.hpp"
#include "Log.hpp"
#include "ShaderManager.hpp"
#include "Rendering/GLUtils.hpp"

namespace Astera {
    void CommandQueue::ExecuteQueue() {
        CommandExecutor executor;

        for (const auto& command : mCommands) {
            std::visit(executor, command);
        }

        Clear();
    }

    void CommandQueue::ExecuteQueueBatched() {
        if (!mBatchResourcesInitialized) {
            Log::Error("CommandQueue", "Batch resources not initialized");
            ExecuteQueue();  // Fallback to non-batched
            return;
        }

        CommandExecutor executor;

        // Execute non-sprite commands first
        for (const auto& command : mCommands) {
            if (!std::holds_alternative<DrawSpriteCommand>(command)) {
                std::visit(executor, command);
            }
        }

        // Batch and render sprites
        BatchSpriteCommands();

        for (const auto& batch : mBatches) {
            RenderBatch(batch);
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

    void CommandQueue::BatchSpriteCommands() {
        mBatches.clear();

        // Extract sprite commands and sort by texture
        vector<size_t> spriteIndices;
        for (size_t i = 0; i < mCommands.size(); ++i) {
            if (std::holds_alternative<DrawSpriteCommand>(mCommands[i])) {
                spriteIndices.push_back(i);
            }
        }

        if (spriteIndices.empty())
            return;

        // Stable sort to maintain draw order for same texture
        std::ranges::stable_sort(spriteIndices, [this](size_t a, size_t b) {
            const auto& cmdA = std::get<DrawSpriteCommand>(mCommands[a]);
            const auto& cmdB = std::get<DrawSpriteCommand>(mCommands[b]);
            return cmdA.sprite.textureId < cmdB.sprite.textureId;
        });

        // Build batches
        SpriteBatch currentBatch;
        currentBatch.quadVAO = mBatchVAO;
        u32 currentTexture   = static_cast<u32>(-1);

        for (const size_t idx : spriteIndices) {
            const auto& cmd = std::get<DrawSpriteCommand>(mCommands[idx]);

            // Start new batch if texture changes or batch is full
            if (cmd.sprite.textureId != currentTexture || currentBatch.SpriteCount() >= kMaxSpritesPerBatch) {
                if (!currentBatch.instances.empty()) {
                    mBatches.push_back(std::move(currentBatch));
                    currentBatch         = SpriteBatch();
                    currentBatch.quadVAO = mBatchVAO;
                }

                currentTexture         = cmd.sprite.textureId;
                currentBatch.textureId = currentTexture;
            }

            // Calculate MVP transform for this sprite
            const Mat4 model      = cmd.transform.GetMatrix();
            const Mat4 projection = Coordinates::CreateScreenProjection(cmd.screenDimensions.x, cmd.screenDimensions.y);
            const Mat4 mvp        = projection * model;

            // Add instance data
            currentBatch.instances.emplace_back(mvp, cmd.tintColor);
        }

        // Add final batch
        if (!currentBatch.instances.empty()) {
            mBatches.push_back(std::move(currentBatch));
        }

        // Log::Debug("CommandQueue", "Batched {} sprites into {} draw calls", spriteIndices.size(), mBatches.size());
    }

    void CommandQueue::RenderBatch(const SpriteBatch& batch) const {
        if (batch.instances.empty())
            return;

        ASTERA_ASSERT(mBatchResourcesInitialized);
        ASTERA_ASSERT(batch.SpriteCount() <= kMaxSpritesPerBatch);

        // Upload instance data
        mInstanceVBO->Bind();
        mInstanceVBO->UpdateData(batch.instances.data(), batch.instances.size() * sizeof(SpriteInstanceData), 0);

        // Bind shader
        const auto shader = ShaderManager::GetShader(Shaders::SpriteInstanced);
        ASTERA_ASSERT(shader);
        shader->Bind();

        // Bind texture
        GLCall(glActiveTexture, GL_TEXTURE0);
        GLCall(glBindTexture, GL_TEXTURE_2D, batch.textureId);
        shader->SetUniform("uSprite", 0);

        // Draw instanced
        batch.quadVAO->Bind();
        GLCall(glDrawElementsInstanced,
               GL_TRIANGLES,
               6,  // 6 indices per quad
               GL_UNSIGNED_INT,
               nullptr,
               CAST<GLsizei>(batch.SpriteCount()));

        shader->Unbind();
    }

    void CommandQueue::InitializeBatchResources() {
        if (mBatchResourcesInitialized)
            return;

        Log::Debug("CommandQueue", "Initializing sprite batching resources...");

        // Create shared quad geometry (all sprites share this)
        const SpriteVertex quadVertices[] = {
          {-0.5f, -0.5f, 0.0f, 0.0f},  // Bottom-left
          {0.5f, -0.5f, 1.0f, 0.0f},   // Bottom-right
          {-0.5f, 0.5f, 0.0f, 1.0f},   // Top-left
          {0.5f, 0.5f, 1.0f, 1.0f},    // Top-right
        };

        const u32 quadIndices[] = {
          0,
          1,
          2,  // First triangle
          2,
          1,
          3  // Second triangle
        };

        // Create quad vertex buffer
        mQuadVBO = make_shared<VertexBuffer>();
        mQuadVBO->SetData(quadVertices, sizeof(quadVertices), BufferUsage::Static);

        // Create quad index buffer
        mQuadIBO = make_shared<IndexBuffer>();
        mQuadIBO->SetIndices(quadIndices, 6, BufferUsage::Static);

        // Create instance data buffer (dynamic, updated per frame)
        const size_t instanceBufferSize = kMaxSpritesPerBatch * sizeof(SpriteInstanceData);
        mInstanceVBO                    = make_shared<VertexBuffer>();
        mInstanceVBO->SetData(nullptr, instanceBufferSize, BufferUsage::Stream);

        // Setup VAO
        mBatchVAO = make_shared<VertexArray>();

        // Add quad vertices (per-vertex attributes)
        VertexLayout quadLayout;
        quadLayout.AddAttribute(VertexAttribute("aVertex", AttributeType::Float4));
        mBatchVAO->AddVertexBuffer(mQuadVBO, quadLayout);

        // Add instance data (per-instance attributes)
        // Note: We'll set up instanced attributes manually since VertexLayout doesn't support divisors yet
        mBatchVAO->Bind();
        mInstanceVBO->Bind();

        // Layout for SpriteInstanceData:
        // - Mat4 transform (location 1-4, 4 vec4s)
        // - Vec4 tintColor (location 5)

        const size_t mat4Size = sizeof(Mat4);
        const size_t vec4Size = sizeof(Vec4);
        const size_t stride   = sizeof(SpriteInstanceData);

        // Setup transform matrix (takes 4 attribute locations)
        for (u32 i = 0; i < 4; ++i) {
            GLCall(glEnableVertexAttribArray, 1 + i);
            GLCall(glVertexAttribPointer,
                   1 + i,
                   4,
                   GL_FLOAT,
                   GL_FALSE,
                   CAST<GLsizei>(stride),
                   RCAST<void*>(i * vec4Size));
            GLCall(glVertexAttribDivisor, 1 + i, 1);  // One per instance
        }

        // Setup tint color
        GLCall(glEnableVertexAttribArray, 5);
        GLCall(glVertexAttribPointer, 5, 4, GL_FLOAT, GL_FALSE, CAST<GLsizei>(stride), RCAST<void*>(mat4Size));
        GLCall(glVertexAttribDivisor, 5, 1);  // One per instance

        // Set index buffer
        mBatchVAO->SetIndexBuffer(mQuadIBO);

        VertexArray::Unbind();

        mBatchResourcesInitialized = true;
        Log::Debug("CommandQueue", "Sprite batching initialized (max {} sprites per batch)", kMaxSpritesPerBatch);
    }

    void CommandQueue::Reset() {
        Clear();
        mBatches.clear();
        mBatchVAO.reset();
        mQuadVBO.reset();
        mQuadIBO.reset();
        mInstanceVBO.reset();
        mBatchResourcesInitialized = false;
    }

    // ============================================================================
    // CommandExecutor Implementation
    // ============================================================================

    void CommandExecutor::operator()(const ClearCommand& cmd) const {
        GLbitfield clearFlags = GL_COLOR_BUFFER_BIT;

        GLCall(glClearColor, cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);

        if (cmd.clearDepth) {
            clearFlags |= GL_DEPTH_BUFFER_BIT;
        }

        if (cmd.clearStencil) {
            clearFlags |= GL_STENCIL_BUFFER_BIT;
        }

        GLCall(glClear, clearFlags);
    }

    void CommandExecutor::operator()(const DrawSpriteCommand& cmd) const {
        const auto spriteShader = ShaderManager::GetShader(Shaders::Sprite);
        ASTERA_ASSERT(spriteShader);
        spriteShader->Bind();

        GLCall(glActiveTexture, GL_TEXTURE0);
        GLCall(glBindTexture, GL_TEXTURE_2D, cmd.sprite.textureId);
        spriteShader->SetUniform("uSprite", 0);

        const Mat4 model      = cmd.transform.GetMatrix();
        const Mat4 projection = Coordinates::CreateScreenProjection(cmd.screenDimensions.x, cmd.screenDimensions.y);
        const Mat4 mvp        = projection * model;
        spriteShader->SetUniform("uMVP", mvp);

        const auto drawCmd = DrawIndexedCommand {
          .vao           = cmd.sprite.geometry->GetVertexArray(),
          .indexCount    = CAST<u32>(cmd.sprite.geometry->GetVertexArray()->GetIndexBuffer()->GetCount()),
          .primitiveType = GL_TRIANGLES,
          .indexOffset   = 0};

        (*this)(drawCmd);
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

    void CommandExecutor::operator()(const DrawIndexedCommand& cmd) const {
        ASTERA_ASSERT(cmd.vao != nullptr);
        ASTERA_ASSERT(cmd.vao->GetIndexBuffer() != nullptr);

        cmd.vao->Bind();

        const void* indexOffset = RCAST<void*>(CAST<uptr>(cmd.indexOffset * sizeof(u32)));

        GLCall(glDrawElements, cmd.primitiveType, CAST<GLsizei>(cmd.indexCount), GL_UNSIGNED_INT, indexOffset);
    }

    void CommandExecutor::operator()(const DrawIndexedInstancedCommand& cmd) const {
        ASTERA_ASSERT(cmd.vao != nullptr);
        ASTERA_ASSERT(cmd.vao->GetIndexBuffer() != nullptr);
        ASTERA_ASSERT(cmd.instanceCount > 0);

        cmd.vao->Bind();

        const void* indexOffset = RCAST<void*>(CAST<uptr>(cmd.indexOffset * sizeof(u32)));

        GLCall(glDrawElementsInstanced,
               cmd.primitiveType,
               CAST<GLsizei>(cmd.indexCount),
               GL_UNSIGNED_INT,
               indexOffset,
               CAST<GLsizei>(cmd.instanceCount));
    }

    void CommandExecutor::operator()(const DrawArraysCommand& cmd) const {
        ASTERA_ASSERT(cmd.vao != nullptr);
        ASTERA_ASSERT(cmd.vertexCount > 0);

        cmd.vao->Bind();

        GLCall(glDrawArrays, cmd.primitiveType, CAST<GLint>(cmd.vertexOffset), CAST<GLsizei>(cmd.vertexCount));
    }

    void CommandExecutor::operator()(const UpdateVertexBufferCommand& cmd) const {
        ASTERA_ASSERT(cmd.buffer != nullptr);
        ASTERA_ASSERT(!cmd.data.empty());

        cmd.buffer->Bind();
        cmd.buffer->UpdateData(cmd.data.data(), cmd.data.size(), cmd.offset);
    }

    void CommandExecutor::operator()(const UpdateIndexBufferCommand& cmd) const {
        ASTERA_ASSERT(cmd.buffer != nullptr);
        ASTERA_ASSERT(!cmd.indices.empty());

        cmd.buffer->Bind();
        cmd.buffer->UpdateData(cmd.indices.data(), cmd.indices.size() * sizeof(u32), cmd.offset);
    }

    void CommandExecutor::operator()(const BindVertexArrayCommand& cmd) const {
        ASTERA_ASSERT(cmd.vao != nullptr);
        cmd.vao->Bind();
    }

    void CommandExecutor::operator()(const UnbindVertexArrayCommand& cmd) const {
        ASTERA_UNUSED(cmd);
        VertexArray::Unbind();
    }
}  // namespace Astera