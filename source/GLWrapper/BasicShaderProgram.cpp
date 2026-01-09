#include "BasicShaderProgram.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "Debug/Logger.h"

using namespace GL;

const std::string BasicShaderProgram::SHADER_EXTENSION_VERT = ".vert";
const std::string BasicShaderProgram::SHADER_EXTENSION_FRAG = ".frag";
const std::string BasicShaderProgram::SHADER_DIRECTORY = Shader::SHADER_DEFAULT_DIRECTORY + "RenderShaders/";

BasicShaderProgram::BasicShaderProgram(const char* shaderPathName)
    : BasicShaderProgram(
        (shaderPathName + SHADER_EXTENSION_VERT).c_str(),
        (shaderPathName + SHADER_EXTENSION_FRAG).c_str(),
        std::string(shaderPathName)) { }

BasicShaderProgram::BasicShaderProgram(const char* vertexPath, const char* fragmentPath, std::string shaderName)
{
    this->name = shaderName.empty() ? "Unnamed Render Shader" : shaderName;
    std::string printShaderName = shaderName.empty() ? "[Unnamed Render Shader] " : "[" + shaderName + "] ";

    std::string trueVertexPathStr = SHADER_DIRECTORY + std::string(vertexPath);
    std::string trueFragmentPathStr = SHADER_DIRECTORY + std::string(fragmentPath);
    const char* trueVertexPath = trueVertexPathStr.c_str();
    const char* trueFragmentPath = trueFragmentPathStr.c_str();

    // Load shader from files
    std::string vertexCode = this->LoadFileWithShaderPreprocessor(trueVertexPath, printShaderName);

    this->preprocessorAtFirstLine = true;   //reset preprocessor to compile another shader
    std::string fragmentCode = this->LoadFileWithShaderPreprocessor(trueFragmentPath, printShaderName);

    const char* vertexCodeCStr = vertexCode.c_str();
    const char* fragmentCodeCStr = fragmentCode.c_str();

    // Compile loaded shader
    GLuint vertexShader = this->CompileShader(vertexCodeCStr, printShaderName, GL::ShaderType::Vertex);
    GLuint fragmentShader = this->CompileShader(fragmentCodeCStr, printShaderName, GL::ShaderType::Fragment);

    // Create shader program
    GLint success;
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        Debug::LogError(printShaderName + "Error linking rendering shader program: " + infoLog);
        ID = 0; // Prevent use of invalid shader
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
