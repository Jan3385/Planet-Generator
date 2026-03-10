#include "ComplexShaderProgram.h"

#include "Debug/Logger.h"

/// @brief Adds a shader to a queue used to compile the shader program
/// @param type Type of shader that is being added
/// @param path Path of the shader (with extension). ex. "lighting.vert"
void GL::ComplexShaderProgram::AddShader(GL::ShaderType type, const char *path)
{
    if(this->compiled) [[unlikely]] {
        Debug::LogError(this->name + ": Cannot add shader to an already compiled shader program!");
        return;
    }

    this->uncompiledShaders.emplace(ShaderInfo(type, path));
}

/// @brief Compiles the shader program with the added shaders in sequence
void GL::ComplexShaderProgram::Compile()
{
    if(this->compiled) [[unlikely]] {
        Debug::LogError(this->name + ": Shader was already compiled!");
        return;
    }
    if(this->uncompiledShaders.empty()) [[unlikely]] {
        Debug::LogError(this->name + ": Cannot compile a complex shader program with no attached shaders!");
        return;
    }
    if(this->ID != 0) [[unlikely]] {
        Debug::LogError(this->name + ": Shader was already compiled!");
        return;
    }

    // Create shader program
    GLint success;
    this->ID = glCreateProgram();

    // compile & load shaders in sequence
    std::queue<GLuint> shaderIDs;
    while(!this->uncompiledShaders.empty())
    {
        ShaderInfo f = this->uncompiledShaders.front();

        std::string shaderCode = this->LoadFileWithShaderPreprocessor(f.path, this->name);
        GLuint s = this->CompileShader(shaderCode.c_str(), this->name, f.type);
        glAttachShader(this->ID, s);
        shaderIDs.emplace(s);

        this->uncompiledShaders.pop();
    }
    
    glLinkProgram(ID);

    // delete all shaders after linking
    while(!shaderIDs.empty())
    {
        glDeleteShader(shaderIDs.front());
        shaderIDs.pop();
    }

    // error handling
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        Debug::LogError(this->name + "Error linking rendering shader program: " + infoLog);
        ID = 0; // Prevent use of invalid shader
    }

    // Update all local shader uniforms
    Shader::activeShaderID = ID;
    glUseProgram(ID);
    for(auto& var : this->localShaderVariables) {
        this->SetUniform(this->PreprocessorVarToUniform(var.first), var.second);
    }

    this->compiled = true;
}

GL::ComplexShaderProgram::ComplexShaderProgram(ComplexShaderProgram &&other) noexcept
    : Shader(std::move(other)), compiled(other.compiled)
{
    this->uncompiledShaders = std::move(other.uncompiledShaders);
}

GL::ComplexShaderProgram &GL::ComplexShaderProgram::operator=(ComplexShaderProgram &&other) noexcept
{
    Shader::operator=(std::move(other));
    this->compiled = other.compiled;
    return *this;
}
