#pragma once

#include "GLWrapper/Shader.h"

#include <queue>

namespace GL{
class ComplexShaderProgram : public GL::Shader {
public:
    ComplexShaderProgram() = default;
    ComplexShaderProgram(std::string shaderName) { this->name = shaderName; };
    ~ComplexShaderProgram() = default;

    void AddShader(GL::ShaderType type, const char* path);
    void Compile();

    // disable copy semantics
    ComplexShaderProgram(const ComplexShaderProgram&) = delete;
    ComplexShaderProgram& operator=(const ComplexShaderProgram&) = delete;

    // move semantics
    ComplexShaderProgram(ComplexShaderProgram&&other) noexcept;
    ComplexShaderProgram& operator=(ComplexShaderProgram&&other) noexcept;
private:
    struct ShaderInfo{
        GL::ShaderType type;
        const char* path;
    };
    std::queue<ShaderInfo> uncompiledShaders;
    bool compiled = false;
};
}