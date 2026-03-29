#pragma once

#include <unordered_map>
#include <memory>

#include "Object/Material/Material.h"
#include "GLWrapper/Shader.h"

class MaterialLibrary{
public:
    MaterialLibrary();

    Object::Material* GetDefaultMaterial() const { return this->GetMaterial("default"); }

    Object::Material* CreateMaterial(const std::string& name, GL::Shader* materialShader);
    Object::Material* GetMaterial(const std::string& name) const;

    bool MaterialExists(const std::string& name) const;
    
    /// @brief Removes a material from the library
    /// @param name name of the material
    /// @warning If the material is used by any render component, it can cause a crash!
    void RemoveMaterial(const std::string& name);

    Object::Material::cloneID GenCloneID() { return cloneCounter++; }

    /// @brief Generates a name from a clone ID
    /// @param cloneID unique ID of the clone
    /// @return a unique name for the clone
    static std::string GenCloneName(Object::Material::cloneID cloneID) { return "clone_" + std::to_string(cloneID); }
private:
    std::unordered_map<std::string, std::unique_ptr<Object::Material>> materials;
    Object::Material::cloneID cloneCounter = 0;
};