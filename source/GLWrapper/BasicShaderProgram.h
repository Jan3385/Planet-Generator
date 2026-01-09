#pragma once

#include "GLWrapper/Shader.h"

namespace GL{
    class BasicShaderProgram : public GL::Shader {
    public:
        BasicShaderProgram() = default;
        BasicShaderProgram(const char* shaderPathName);
        BasicShaderProgram(const char* vertexPath, const char* fragmentPath, std::string shaderName);
        ~BasicShaderProgram() = default;
    private:
        static const std::string SHADER_EXTENSION_VERT;
        static const std::string SHADER_EXTENSION_FRAG;
        static const std::string SHADER_DIRECTORY;
    };
}