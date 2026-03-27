#include "Material.h"

Object::Material::Material(GL::Shader *shader) :
    shader(shader)
{
    
}

void Object::Material::Bind(GL::Shader &shader) const
{
    for (const auto& [name, value] : this->localShaderValues) {
        shader.SetUniform(name, value);
    }
}

Object::Material *Object::Material::SetValue(const std::string &name, const GL::Shader::uniformValue &value)
{
    this->localShaderValues[name] = value;
    return this;
}

/// @brief Return a shader uniform value present with the given name
/// @param name name of the shader value
/// @return the shader value if found, otherwise 0
GL::Shader::uniformValue Object::Material::GetValue(const std::string &name) const
{
    auto it = this->localShaderValues.find(name);
    if (it != this->localShaderValues.end()) {
        return it->second;
    }
    return 0;
}