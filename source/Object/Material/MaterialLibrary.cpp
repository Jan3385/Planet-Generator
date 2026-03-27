#include "MaterialLibrary.h"

#include "Engine/Engine.h"

MaterialLibrary::MaterialLibrary()
{
    using namespace Object;
    Material* defaultMat = this->CreateMaterial("default", &GameEngine::renderer->GetDefaultLightShader());
    defaultMat->attributes = Material::RenderAttributes::NormalMatrix | Material::RenderAttributes::Transform;
    defaultMat->transparency = Material::Transparency::Opaque;
    defaultMat->SetValue("material.color", glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
    defaultMat->SetValue("material.MetalRough", glm::vec4(0.0f, 0.5f, 0.0f, 0.0f));
}

Object::Material *MaterialLibrary::CreateMaterial(const std::string &name, GL::Shader *materialShader)
{
    auto mat = std::make_unique<Object::Material>(materialShader);
    Object::Material* matPtr = mat.get();
    this->materials[name] = std::move(mat);
    return matPtr;
}

/// @brief Gets a material from the library
/// @param name name of the material
/// @return Pointer to the material if found, nullptr otherwise
Object::Material *MaterialLibrary::GetMaterial(const std::string &name) const
{
    auto it = this->materials.find(name);
    return (it != this->materials.end()) ? it->second.get() : nullptr;
}

bool MaterialLibrary::MaterialExists(const std::string &name) const
{
    return this->materials.find(name) != this->materials.end();
}

void MaterialLibrary::RemoveMaterial(const std::string &name)
{
    this->materials.erase(name);
}
