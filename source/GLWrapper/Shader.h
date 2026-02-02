#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <variant>

namespace GL
{
enum ShaderType : GLenum {
    Vertex   = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Geometry = GL_GEOMETRY_SHADER,
    Compute  = GL_COMPUTE_SHADER,
    TessControl = GL_TESS_CONTROL_SHADER,
    TessEvaluation = GL_TESS_EVALUATION_SHADER
};
extern const std::unordered_map<ShaderType, std::string> ShaderTypeToString;

class Shader{
public:
    // Shader program or Compute shader based on context
    Shader() = default;
    ~Shader();

    // disable copy semantics
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // move semantics
    Shader(Shader&&other) noexcept;
    Shader& operator=(Shader&&other) noexcept;

    std::string LoadFileWithShaderPreprocessor(const std::string& filePath, const std::string& shaderName);

    static void UnsetActiveShaderCache();
    void Use();

    GLint GetUniformLocation(const std::string &name);

    using uniformValue = 
        std::variant<bool, unsigned int, int, float, 
            glm::mat3, glm::mat4, glm::vec2, glm::ivec2, glm::vec3, glm::vec4>;


    static void AddShaderConstant(const std::string& key, const std::string& value);
    static void AddShaderVariable(const std::string& key, const uniformValue& value);
    static void UpdateShaderVariable(const std::string& key, const uniformValue& value);

    // Uniform setters
    void SetBool(const std::string &name, bool value);
    void SetUnsignedInt(const std::string &name, unsigned int value);
    void SetInt(const std::string &name, int value);
    void SetFloat(const std::string &name, float value);
    void SetMat3(const std::string &name, glm::mat3 value);
    void SetMat4(const std::string &name, glm::mat4 value);
    void SetVec2(const std::string &name, glm::vec2 value);
    void SetIVec2(const std::string &name, glm::ivec2 value);
    void SetVec3(const std::string &name, glm::vec3 value);
    void SetVec4(const std::string &name, glm::vec4 value);

    template<typename T>
    void SetUniformAny(const std::string &name, const T &value);
    void SetUniform(const std::string &name, const uniformValue &value);
protected:
    GLuint ID = 0;

    bool preprocessorAtFirstLine = true;
    static const std::string SHADER_DEFAULT_DIRECTORY;
    std::string name;
    GLuint CompileShader(const char* shaderSource, const std::string& shaderName, ShaderType shaderType);

    std::string PreprocessorVarToUniform(const std::string& variableName);
    std::unordered_map<std::string, uniformValue> localShaderVariables;

    static GLuint activeShaderID;
private:
    //format: ###
    static const std::string SHADER_VERSION;
    static const std::string SHADER_INCLUDE_EXTENSION;
    static const std::string SHADER_INCLUDE_DIRECTORY;

    static std::unordered_map<std::string, std::string> shaderConstants;
    static std::unordered_map<std::string, uniformValue> shaderVariables;

    std::unordered_set<std::string> includedFiles;
    std::unordered_map<std::string, GLint> uniformLocationCache;

    void PreprocessorHandleInclude(const std::string& line, std::stringstream& source, const std::string& shaderName);
    void PreprocessorHandleGet(const std::string& line, std::stringstream& source, const std::string& shaderName);
    void PreprocessorHandleVar(const std::string& line, std::stringstream& source, const std::string& shaderName);
};
}

template<typename T>
void GL::Shader::SetUniformAny(const std::string &name, const T &value)
{
    if constexpr (std::is_same_v<T, bool>) {
        this->SetBool(name, value);
    } else if constexpr (std::is_same_v<T, unsigned int>) {
        this->SetUnsignedInt(name, value);
    } else if constexpr (std::is_same_v<T, int>) {
        this->SetInt(name, value);
    } else if constexpr (std::is_same_v<T, float>) {
        this->SetFloat(name, value);
    } else if constexpr (std::is_same_v<T, glm::mat3>) {
        this->SetMat3(name, value);
    } else if constexpr (std::is_same_v<T, glm::mat4>) {
        this->SetMat4(name, value);
    } else if constexpr (std::is_same_v<T, glm::vec2>) {
        this->SetVec2(name, value);
    } else if constexpr (std::is_same_v<T, glm::ivec2>) {
        this->SetIVec2(name, value);
    } else if constexpr (std::is_same_v<T, glm::vec3>) {
        this->SetVec3(name, value);
    } else if constexpr (std::is_same_v<T, glm::vec4>) {
        this->SetVec4(name, value);
    } else {
        static_assert(!std::is_same_v<T, T>, "Unsupported uniform type in SetUniformAny");
    }
}
