#pragma once

#include "GLWrapper/Shader.h"

namespace Object {
class Material {
public:
    enum class Transparency{
        Special = 0, // rendering callback will not be automatically set
        Opaque = 1,
        Transparent = 2,
        OpaqueNoLight,
    };
    // flags for shader attributes that will be passed to the shader
    enum class RenderAttributes{
        None                    = 0,
        Transform               = 1 << 0,
        PreviousTransform       = 1 << 1,
        NormalMatrix            = 1 << 2,
        ReverseFaceCulling      = 1 << 3,
        OriginPos               = 1 << 4,
        ViewMatrix              = 1 << 5,
        Default                 = Transform | NormalMatrix,
    };

    Material(GL::Shader* shader);
    ~Material() = default;

    bool HasAttribute(RenderAttributes attribute) const { return (static_cast<int>(this->attributes) & static_cast<int>(attribute)) != static_cast<int>(RenderAttributes::None); }
    GL::Shader* GetShader() const { return this->shader; }

    void Bind(GL::Shader& shader) const;

    Material* SetValue(const std::string& name, const GL::Shader::uniformValue& value);
    GL::Shader::uniformValue GetValue(const std::string& name) const;

    Material* SetTransparency(Transparency transparency) { this->transparency = transparency; return this; }

    Transparency transparency = Transparency::Opaque;

    // flags for shader attributes
    RenderAttributes attributes = RenderAttributes::None;
private:
    GL::Shader* shader;
    std::unordered_map<std::string, GL::Shader::uniformValue> localShaderValues;
};
}

// bitwise operators for Material::RenderAttributes ----
inline Object::Material::RenderAttributes operator|(
    Object::Material::RenderAttributes left,
    Object::Material::RenderAttributes right)
{
    using T = std::underlying_type_t<Object::Material::RenderAttributes>;
    return static_cast<Object::Material::RenderAttributes>(
        static_cast<T>(left) | static_cast<T>(right));
}

inline Object::Material::RenderAttributes operator&(
    Object::Material::RenderAttributes left,
    Object::Material::RenderAttributes right)
{
    using T = std::underlying_type_t<Object::Material::RenderAttributes>;
    return static_cast<Object::Material::RenderAttributes>(
        static_cast<T>(left) & static_cast<T>(right));
}
// -----------------------------------------------------