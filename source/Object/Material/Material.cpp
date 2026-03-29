#include "Material.h"

#include "Object/Material/MaterialLibrary.h"
#include "Engine/Engine.h"

Object::Material::Material(GL::Shader *shader) :
    shader(shader)
{
    
}

void Object::Material::Bind(GL::Shader &shader) const
{
    for (const auto& [name, value] : this->localShaderValues) {
        shader.SetUniform(name, value);
    }

    for (const auto& [name, bufferBindingPair] : this->uniformBuffers) {
        bufferBindingPair.first->BindBufferBase(bufferBindingPair.second);
    }

    if(this->texture) this->texture->Bind();
}

/// @brief Set a shader uniform value
/// @param name name of the shader value
/// @param value value to be set
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

/// @brief Set a uniform buffer to be used with this material
/// @param name name of the uniform buffer
/// @param buffer pointer to the uniform buffer
/// @param binding  binding point to the uniform buffer
Object::Material *Object::Material::SetUniformBuffer(const std::string &name, GL::BufferBase *buffer, GLuint binding)
{
    this->uniformBuffers[name] = std::make_pair(buffer, binding);
    return this;
}

/// @brief Return a uniform buffer present with the given name
/// @param name name of the uniform buffer
/// @return a pair of pointer to the uniform buffer and its binding if found, otherwise {nullptr, 0}
std::pair<GL::BufferBase *, GLuint> Object::Material::GetUniformBuffer(const std::string &name) const
{
    auto it = this->uniformBuffers.find(name);
    if (it != this->uniformBuffers.end()) {
        return it->second;
    }
    return {nullptr, 0};
}

/// @brief  Create a clone of this material with a unique name
/// @param outputCloneID outputCloneID will store the unique ID of the clone, which can be used to get the material from the library
/// @return pointer to the new cloned material
Object::Material *Object::Material::Clone(cloneID *outputCloneID) const
{
    cloneID newCloneID = GameEngine::materialLibrary->GenCloneID();
    if(outputCloneID) *outputCloneID = newCloneID;
    std::string uniqueName = MaterialLibrary::GenCloneName(newCloneID);

    Material* clone = GameEngine::materialLibrary->CreateMaterial(uniqueName, this->shader);
    clone->texture = this->texture;
    clone->transparency = this->transparency;
    clone->attributes = this->attributes;
    clone->localShaderValues = this->localShaderValues;
    clone->uniformBuffers = this->uniformBuffers;
    return clone;
}
