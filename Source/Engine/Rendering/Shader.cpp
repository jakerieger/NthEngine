// Author: Jake Rieger
// Created: 11/29/25.
//

#include "Shader.hpp"

namespace Nth {
    Shader::~Shader() {
        GLCall(glDeleteProgram, mProgram);
    }

    void Shader::FromFile(const fs::path& vertexFile, const fs::path& fragFile) {
        N_ASSERT(fs::exists(vertexFile) && fs::exists(fragFile));

        const string vertexSource = IO::ReadString(vertexFile);
        const string fragSource   = IO::ReadString(fragFile);

        CompileShaders(vertexSource.c_str(), fragSource.c_str());
    }

    void Shader::FromMemory(const char* vertexSource, const char* fragSource) {
        CompileShaders(vertexSource, fragSource);
    }

    void Shader::Bind() {
        GLCall(glUseProgram, mProgram);
    }

    void Shader::Unbind() const {
        N_UNUSED(this);
        GLCall(glUseProgram, 0);
    }

    void Shader::CompileShaders(const char* vertexSource, const char* fragSource) {
        const u32 vertexShader = GLCall(glCreateShader, GL_VERTEX_SHADER);
        GLCall(glShaderSource, vertexShader, 1, &vertexSource, nullptr);
        GLCall(glCompileShader, vertexShader);

        const u32 fragmentShader = GLCall(glCreateShader, GL_FRAGMENT_SHADER);
        GLCall(glShaderSource, fragmentShader, 1, &fragSource, nullptr);
        GLCall(glCompileShader, fragmentShader);

        mProgram = glCreateProgram();
        GLCall(glAttachShader, mProgram, vertexShader);
        GLCall(glAttachShader, mProgram, fragmentShader);
        GLCall(glLinkProgram, mProgram);
        GLCall(glValidateProgram, mProgram);

        GLCall(glDeleteShader, vertexShader);
        GLCall(glDeleteShader, fragmentShader);
    }
}  // namespace Nth