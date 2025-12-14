/*
 *  Filename: PhysicsDebugLayer.cpp
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

#include "PhysicsDebugLayer.hpp"
#include "Math.hpp"
#include "Log.hpp"
#include "Rendering/GLUtils.hpp"

namespace Astera {
    static Color GetRandomColor() {
        static constexpr size_t colorCount {5};
        static Color colors[colorCount] = {Colors::Red, Colors::Green, Colors::Cyan, Colors::Magenta, Colors::Yellow};
        const auto randomIndex          = Math::RandomInt(0, (i32)colorCount - 1);
        return colors[randomIndex];
    }

    PhysicsDebugLayer::PhysicsDebugLayer(u32 width, u32 height) : mWidth(width), mHeight(height) {
        InitShaders();
        SetupBuffers();
    }

    PhysicsDebugLayer::~PhysicsDebugLayer() {
        GLCall(glDeleteVertexArrays, 1, &mVAO);
        GLCall(glDeleteBuffers, 1, &mVBO);
        GLCall(glDeleteProgram, mShaderProgram);
    }

    void PhysicsDebugLayer::OnUpdate(float deltaTime) {}

    void PhysicsDebugLayer::OnRender() {
        GLCall(glClear, GL_DEPTH_BUFFER_BIT);
        GLCall(glUseProgram, mShaderProgram);
        GLCall(glBindVertexArray, mVAO);

        {
            for (const auto transform : mTransforms) {
                // Draw a rectangle around each object
                static auto color = GetRandomColor();
                const auto posX   = transform.position.x - (transform.scale.x / 2);
                const auto posY   = transform.position.y - (transform.scale.y / 2);
                DrawRectangle(posX, posY, transform.scale.x, transform.scale.y, color, false);
            }
        }

        if (mVAO != 0) GLCall(glBindVertexArray, 0);
        if (mShaderProgram != 0) GLCall(glUseProgram, 0);
    }

    void PhysicsDebugLayer::OnEvent(const Event& event) {}

    void PhysicsDebugLayer::InitShaders() {
        const GLuint vertexShader = GLCall(glCreateShader, GL_VERTEX_SHADER);
        const char* vertexSource  = kVertexShaderSource.data();
        GLCall(glShaderSource, vertexShader, 1, &vertexSource, nullptr);
        GLCall(glCompileShader, vertexShader);

        GLint success;
        GLCall(glGetShaderiv, vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            GLCall(glGetShaderInfoLog, vertexShader, 512, nullptr, infoLog);
            Log::Error("PhysicsDebugLayer", "Vertex shader compilation failed: {}", infoLog);
        }

        const GLuint fragmentShader = GLCall(glCreateShader, GL_FRAGMENT_SHADER);
        const char* fragSource      = kFragmentShaderSource.data();
        GLCall(glShaderSource, fragmentShader, 1, &fragSource, nullptr);
        GLCall(glCompileShader, fragmentShader);

        GLCall(glGetShaderiv, fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            GLCall(glGetShaderInfoLog, fragmentShader, 512, nullptr, infoLog);
            Log::Error("PhysicsDebugLayer", "Fragment shader compilation failed: {}", infoLog);
        }

        mShaderProgram = GLCall(glCreateProgram);
        GLCall(glAttachShader, mShaderProgram, vertexShader);
        GLCall(glAttachShader, mShaderProgram, fragmentShader);
        GLCall(glLinkProgram, mShaderProgram);

        GLCall(glGetProgramiv, mShaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            GLCall(glGetProgramInfoLog, mShaderProgram, 512, nullptr, infoLog);
            Log::Error("PhysicsDebugLayer", "Shader program linking failed: {}", infoLog);
        }

        GLCall(glDeleteShader, vertexShader);
        GLCall(glDeleteShader, fragmentShader);

        mColorLocation = GLCall(glGetUniformLocation, mShaderProgram, "uColor");
    }

    void PhysicsDebugLayer::SetupBuffers() {
        GLCall(glGenVertexArrays, 1, &mVAO);
        GLCall(glGenBuffers, 1, &mVBO);

        GLCall(glBindVertexArray, mVAO);
        GLCall(glBindBuffer, GL_ARRAY_BUFFER, mVBO);

        // Position attribute
        GLCall(glVertexAttribPointer, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void*)nullptr);
        GLCall(glEnableVertexAttribArray, 0);

        GLCall(glBindBuffer, GL_ARRAY_BUFFER, 0);
        GLCall(glBindVertexArray, 0);
    }

    void PhysicsDebugLayer::DrawVertices(const vector<f32>& vertices, GLenum mode) const {
        GLCall(glBindBuffer, GL_ARRAY_BUFFER, mVBO);
        GLCall(glBufferData, GL_ARRAY_BUFFER, vertices.size() * sizeof(f32), vertices.data(), GL_DYNAMIC_DRAW);
        GLCall(glDrawArrays, mode, 0, vertices.size() / 2);
    }

    f32 PhysicsDebugLayer::ScreenToClipX(f32 x) const {
        return (2.0f * x / (f32)mWidth) - 1.0f;
    }

    f32 PhysicsDebugLayer::ScreenToClipY(f32 y) const {
        return (2.0f * y / (f32)mHeight) - 1.0f;
    }

    void PhysicsDebugLayer::DrawLine(f32 x0, f32 y0, f32 x1, f32 y1, const Color& color) const {
        const vector<f32> vertices = {ScreenToClipX(x0), ScreenToClipY(y0), ScreenToClipX(x1), ScreenToClipY(y1)};
        GLCall(glUniform4f, mColorLocation, color.R(), color.G(), color.B(), color.A());
        DrawVertices(vertices, GL_LINES);
    }

    void PhysicsDebugLayer::DrawLine(const Vec2& start, const Vec2& end, const Color& color) const {
        DrawLine(start.x, start.y, end.x, end.y, color);
    }

    void PhysicsDebugLayer::DrawRectangle(f32 x, f32 y, f32 width, f32 height, const Color& color, bool filled) const {
        vector<f32> vertices;

        if (filled) {
            // Two triangles for filled rectangle
            vertices = {ScreenToClipX(x),
                        ScreenToClipY(y),
                        ScreenToClipX(x + width),
                        ScreenToClipY(y),
                        ScreenToClipX(x + width),
                        ScreenToClipY(y + height),

                        ScreenToClipX(x),
                        ScreenToClipY(y),
                        ScreenToClipX(x + width),
                        ScreenToClipY(y + height),
                        ScreenToClipX(x),
                        ScreenToClipY(y + height)};

            GLCall(glUniform4f, mColorLocation, color.R(), color.G(), color.B(), color.A());
            DrawVertices(vertices, GL_TRIANGLES);
        } else {
            // Line loop for outline
            vertices = {ScreenToClipX(x),
                        ScreenToClipY(y),
                        ScreenToClipX(x + width),
                        ScreenToClipY(y),
                        ScreenToClipX(x + width),
                        ScreenToClipY(y + height),
                        ScreenToClipX(x),
                        ScreenToClipY(y + height)};

            GLCall(glUniform4f, mColorLocation, color.R(), color.G(), color.B(), color.A());
            DrawVertices(vertices, GL_LINE_LOOP);
        }
    }

    void PhysicsDebugLayer::DrawCircle(f32 x, f32 y, f32 radius, u32 segments, const Color& color, bool filled) const {
        vector<f32> vertices;

        if (filled) {
            // Triangle fan for filled circle
            vertices.push_back(ScreenToClipX(x));
            vertices.push_back(ScreenToClipY(y));

            for (int i = 0; i <= segments; ++i) {
                const f32 angle = 2.0f * Math::kPi * i / segments;
                vertices.push_back(ScreenToClipX(x + radius * cos(angle)));
                vertices.push_back(ScreenToClipY(y + radius * sin(angle)));
            }

            GLCall(glUniform4f, mColorLocation, color.R(), color.G(), color.B(), color.A());
            DrawVertices(vertices, GL_TRIANGLE_FAN);
        } else {
            // Line loop for outline
            for (int i = 0; i < segments; ++i) {
                const f32 angle = 2.0f * Math::kPi * i / segments;
                vertices.push_back(ScreenToClipX(x + radius * cos(angle)));
                vertices.push_back(ScreenToClipY(y + radius * sin(angle)));
            }

            GLCall(glUniform4f, mColorLocation, color.R(), color.G(), color.B(), color.A());
            DrawVertices(vertices, GL_LINE_LOOP);
        }
    }

    void PhysicsDebugLayer::DrawPolygon(const vector<Vec2>& points, const Color& color, bool filled) const {
        if (points.size() < 3) return;

        vector<f32> vertices;

        if (filled) {
            // Simple triangle fan (works for convex polygons)
            for (const auto& point : points) {
                vertices.push_back(ScreenToClipX(point.x));
                vertices.push_back(ScreenToClipY(point.y));
            }

            GLCall(glUniform4f, mColorLocation, color.R(), color.G(), color.B(), color.A());
            DrawVertices(vertices, GL_TRIANGLE_FAN);
        } else {
            for (const auto& point : points) {
                vertices.push_back(ScreenToClipX(point.x));
                vertices.push_back(ScreenToClipY(point.y));
            }

            GLCall(glUniform4f, mColorLocation, color.R(), color.G(), color.B(), color.A());
            DrawVertices(vertices, GL_LINE_LOOP);
        }
    }
}  // namespace Astera