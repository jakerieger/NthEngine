// Author: Jake Rieger
// Created: 11/29/25.
//

#pragma once

#include "CommonPCH.hpp"
#include "GLUtils.hpp"

namespace Nth {
    class Shader {
    public:
        Shader() = default;
        ~Shader();

        void FromFile(const fs::path& vertexFile, const fs::path& fragFile);
        void FromMemory(const char* vertexSource, const char* fragSource);

        void Bind();
        void Unbind() const;

        template<typename T>
        void SetUniform(const char* name, T val) {}

    private:
        GLuint mProgram {0};

        void CompileShaders(const char* vertexSource, const char* fragSource);
    };

    template<>
    inline void Shader::SetUniform(const char* name, bool val) {
        const auto location = GLCall(glGetUniformLocation, mProgram, name);
        GLCall(glUniform1i, location, static_cast<int>(val));
    }

    template<>
    inline void Shader::SetUniform(const char* name, s32 val) {
        const auto location = GLCall(glGetUniformLocation, mProgram, name);
        GLCall(glUniform1i, location, static_cast<int>(val));
    }

    template<>
    inline void Shader::SetUniform(const char* name, f32 val) {
        const auto location = GLCall(glGetUniformLocation, mProgram, name);
        GLCall(glUniform1f, location, val);
    }

    template<>
    inline void Shader::SetUniform(const char* name, const Vec2& val) {
        const auto location = GLCall(glGetUniformLocation, mProgram, name);
        GLCall(glUniform2fv, location, 1, &val[0]);
    }

    template<>
    inline void Shader::SetUniform(const char* name, const Vec3& val) {
        const auto location = GLCall(glGetUniformLocation, mProgram, name);
        GLCall(glUniform3fv, location, 1, &val[0]);
    }

    template<>
    inline void Shader::SetUniform(const char* name, const Vec4& val) {
        const auto location = GLCall(glGetUniformLocation, mProgram, name);
        GLCall(glUniform4fv, location, 1, &val[0]);
    }

    template<>
    inline void Shader::SetUniform(const char* name, const Mat4& val) {
        const auto location = GLCall(glGetUniformLocation, mProgram, name);
        GLCall(glUniformMatrix4fv, location, 1, GL_FALSE, &val[0][0]);
    }
}  // namespace Nth
