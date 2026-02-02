#include "Shader.h"

#include "Debug/Logger.h"
#include "Engine/Lighting.h"
#include "Component/Planet/PlanetGenComponent.h"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace GL;

const std::string GL::Shader::SHADER_VERSION = "460";
const std::string GL::Shader::SHADER_DEFAULT_DIRECTORY = "Shaders/";
const std::string GL::Shader::SHADER_INCLUDE_EXTENSION = ".glsl";
const std::string GL::Shader::SHADER_INCLUDE_DIRECTORY = Shader::SHADER_DEFAULT_DIRECTORY + "Includes/";

std::unordered_map<std::string, std::string> GL::Shader::shaderConstants = {};
std::unordered_map<std::string, GL::Shader::uniformValue> GL::Shader::shaderVariables = {};

const std::unordered_map<ShaderType, std::string> GL::ShaderTypeToString = {
    {ShaderType::Vertex, "Vertex Shader"},
    {ShaderType::Fragment, "Fragment Shader"},
    {ShaderType::Geometry, "Geometry Shader"},
    {ShaderType::Compute, "Compute Shader"},
    {ShaderType::TessControl, "Tessellation Control Shader"},
    {ShaderType::TessEvaluation, "Tessellation Evaluation Shader"}
};

GLuint GL::Shader::activeShaderID = 0;

void GL::Shader::Use()
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        Debug::LogError("[OnUSE-" + this->name + "] GL error: [" + std::to_string(err) + "]");
    }

    if (ID == 0)
        Debug::LogFatal("Shader with ID 0 is invalid. Name: " + this->name);
    
    if(activeShaderID == ID) {
        return; // Already using this shader
    }

    Shader::activeShaderID = ID;
    glUseProgram(ID);

    // Update all local shader variables
    for(auto& var : this->localShaderVariables) {
        // find global equivalent
        auto globalVar = GL::Shader::shaderVariables.find(var.first);

        // if global variable changed update local and uniform
        if(globalVar != GL::Shader::shaderVariables.end() && var.second != globalVar->second) {
            this->SetUniform(this->PreprocessorVarToUniform(var.first), globalVar->second);
            var.second = uniformValue(globalVar->second);
        }
    }
}

GL::Shader::~Shader()
{
    if (ID != 0) {
        glDeleteProgram(ID);
        ID = 0; // Reset ID to prevent accidental use after deletion
    }
}

GL::Shader::Shader(Shader &&other) noexcept
{
    Debug::LogSpam("Moved GL::Shader " + other.name);
    this->ID = other.ID;
    this->name = std::move(other.name);
    this->localShaderVariables = std::move(other.localShaderVariables);
    this->uniformLocationCache = std::move(other.uniformLocationCache);
    other.ID = 0;
    other.name = "MOVED_SHADER";
}

Shader &GL::Shader::operator=(Shader &&other) noexcept
{
    Debug::LogSpam("Moved GL::Shader " + other.name);
    if (this != &other) {
        this->ID = other.ID;
        this->name = std::move(other.name);
        this->localShaderVariables = std::move(other.localShaderVariables);
        this->uniformLocationCache = std::move(other.uniformLocationCache);
        other.ID = 0;
        other.name = "MOVED_SHADER";
    }
    return *this;
}

/// @brief Loads a shader file with support for includes
/// @param filePath The path to the shader file (must include extension and directory)
/// @param shaderName The name of the shader
/// @note Reset Shader::preprocessorAtFirstLine before executing another time
/// @return The shader source code
std::string GL::Shader::LoadFileWithShaderPreprocessor(const std::string &filePath, const std::string &shaderName)
{
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::badbit);

    std::stringstream source;
    try{
        shaderFile.open(filePath);
        
        std::string line;

        // Add version at the start of each shader
        if(this->preprocessorAtFirstLine){
            if(Shader::shaderConstants.find("SHADER_VERSION") != Shader::shaderConstants.end())
                source << "#version " << Shader::shaderConstants.at("SHADER_VERSION") << " core" << "\n";
            else
                source << "#version 460 core" << "\n";

            this->preprocessorAtFirstLine = false;    
        }

        while (std::getline(shaderFile, line))
        {
            // handle #include including
            // #include "filename.glsl"
            if (line.rfind("#include", 0) == 0) {
                PreprocessorHandleInclude(line, source, shaderName);
            }
            // handle #get to define constants
            // #get SKY_COLOR
            else if(line.rfind("#get", 0) == 0){
                PreprocessorHandleGet(line, source, shaderName);         
            }
            // handle #var to declare uniform variables
            // #var mat4 projection
            else if(line.rfind("#var", 0) == 0){
                PreprocessorHandleVar(line, source, shaderName);         
            }
            else {
                source << line << "\n";
            }
        }   
    }
    catch (const std::ifstream::failure& e) {
        Debug::LogError(shaderName + " Error reading shader files: \n" + e.what());
    }

    return source.str();
}

/// @brief Unsets the active shader cache
/// Use this when you cannot guarantee that the shader cache will be valid (calling a shader elsewhere than in Shader::Use())
void GL::Shader::UnsetActiveShaderCache()
{
    Shader::activeShaderID = 0;
}


/***
 * Gets the location of a uniform variable with caching.
 */
GLint GL::Shader::GetUniformLocation(const std::string &name)
{
    // Return uniform if cached
    auto it = uniformLocationCache.find(name);
    if (it != uniformLocationCache.end()) {
        return it->second;
    }

    // Search for uniform location
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        Debug::LogTrace("Uniform '" + name + "' not found in shader program [" + this->name + "]");
    }
    uniformLocationCache[name] = location;
    return location;
}

void GL::Shader::AddShaderConstant(const std::string &key, const std::string &value)
{
    shaderConstants[key] = value;
}

void GL::Shader::AddShaderVariable(const std::string &key, const uniformValue &value)
{
    shaderVariables[key] = value;
}

void GL::Shader::UpdateShaderVariable(const std::string &key, const uniformValue &value)
{
    shaderVariables[key] = value;
}

void GL::Shader::SetBool(const std::string &name, bool value)
{
    glUniform1i(this->GetUniformLocation(name.c_str()), (int)value);
}

void GL::Shader::SetUnsignedInt(const std::string &name, unsigned int value)
{
    glUniform1ui(this->GetUniformLocation(name.c_str()), value);
}

void GL::Shader::SetInt(const std::string &name, int value)
{
    glUniform1i(this->GetUniformLocation(name.c_str()), value);
}

void GL::Shader::SetFloat(const std::string &name, float value)
{
    glUniform1f(this->GetUniformLocation(name.c_str()), value);
}

void GL::Shader::SetMat3(const std::string &name, glm::mat3 value)
{
    glUniformMatrix3fv(this->GetUniformLocation(name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void GL::Shader::SetMat4(const std::string &name, glm::mat4 value)
{
    glUniformMatrix4fv(this->GetUniformLocation(name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void GL::Shader::SetVec2(const std::string &name, glm::vec2 value)
{
    glUniform2fv(this->GetUniformLocation(name.c_str()), 1, &value[0]);
}
void GL::Shader::SetIVec2(const std::string &name, glm::ivec2 value)
{
    glUniform2iv(this->GetUniformLocation(name.c_str()), 1, &value[0]);
}

void GL::Shader::SetVec3(const std::string &name, glm::vec3 value)
{
    glUniform3fv(this->GetUniformLocation(name.c_str()), 1, &value[0]);
}

void GL::Shader::SetVec4(const std::string &name, glm::vec4 value)
{
    glUniform4fv(this->GetUniformLocation(name.c_str()), 1, &value[0]);
}

void GL::Shader::SetUniform(const std::string &name, const uniformValue &value)
{
    std::visit([&](auto&& v) { SetUniformAny(name, v); }, value);
}

GLuint GL::Shader::CompileShader(const char *shaderSource, const std::string &shaderName, ShaderType shaderType)
{
    GLuint shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &shaderSource, NULL);
    glCompileShader(shaderID);

    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        Debug::LogError(std::format(
            "[{}] Error compiling {}: {}", ShaderTypeToString.at(shaderType), shaderName, std::string(infoLog)
        ));
    }

    return shaderID;
}

void GL::Shader::PreprocessorHandleInclude(const std::string &line, std::stringstream &source, const std::string &shaderName)
{
    size_t firstQuote = line.find('"');
    size_t lastQuote = line.find_last_of('"');

    // if quotes are valid and found
    if (firstQuote != std::string::npos && 
        lastQuote != std::string::npos && 
        firstQuote < lastQuote) {

        // Load the included file into the shader source
        std::string includePath = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
        if(includedFiles.count(includePath) <= 0) {
            includedFiles.insert(includePath);

            includePath = Shader::SHADER_INCLUDE_DIRECTORY + includePath;
            source << this->LoadFileWithShaderPreprocessor(includePath, shaderName) << "\n";
        }
    }else{
        Debug::LogWarn(shaderName + ": Invalid include (skipped): " + line);
    }
}

void GL::Shader::PreprocessorHandleGet(const std::string &line, std::stringstream &source, const std::string &shaderName)
{
    size_t spacePos = line.find(' ');
    if (spacePos != std::string::npos) {
        std::string variableName = line.substr(spacePos + 1);

        source << "#define ";

        if(shaderConstants.find(variableName) != shaderConstants.end()) {
            source << variableName << " " << shaderConstants.at(variableName) << "\n";
        }
        else{
            source << "UNKNOWN_VARIABLE " << variableName << "\n";
            Debug::LogWarn(shaderName + ": Unknown variable requested with #get: " + variableName);
        }
    }else{
        Debug::LogWarn(shaderName + ": Invalid #get directive (skipped): " + line);
    }
}

void GL::Shader::PreprocessorHandleVar(const std::string &line, std::stringstream &source, const std::string &shaderName)
{
    size_t spacePos = line.find(' ');
    if (spacePos != std::string::npos) {
        std::string variableName = line.substr(spacePos + 1);

        if(shaderVariables.find(variableName) == shaderVariables.end()){
            Debug::LogWarn(shaderName + ": Unknown variable requested with #var: " + variableName);
            Debug::LogTrace("Available variables:");
            for(auto& var : shaderVariables){
                Debug::LogTrace(var.first);
            }
            return;
        }

        source << "uniform " << variableName << ";\n";

        uniformValue val = shaderVariables.at(variableName);
        
        this->localShaderVariables[variableName] = val;
    }else{
        Debug::LogWarn(shaderName + ": Invalid #var directive (skipped): " + line);
    }
}

std::string GL::Shader::PreprocessorVarToUniform(const std::string &variableName)
{
    // ex. mat4 projection -> projection
    size_t spacePos = variableName.find(' ');
    if (spacePos != std::string::npos) {
        return variableName.substr(spacePos + 1);
    }
    return variableName;
}
