#include "BaseMeshRenderComponent.h"

#include "Engine/Engine.h"

glm::vec3 Component::BaseMeshRender::GetPosition() const
{
    if(!this->transform) return glm::vec3(0.0f);

    return this->transform->GetPos();
}

bool Component::BaseMeshRender::IsSphereInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes, glm::vec3 &centroid, double radius)
{
    for (const auto& plane : frustumPlanes) {
        if (glm::dot(glm::vec3(plane), centroid) + plane.w < -radius) {
            return false;
        }
    }

    return true;
}

/// @brief Sets the render callback based on transparency type
/// @param transparency Transparency type of the material
void Component::BaseMeshRender::SetRenderCallback(Object::Material::Transparency transparency)
{
    this->UnsetRenderCallback();

    this->transparencyCallbackType = transparency;

    switch (transparency)
    {
    case Object::Material::Transparency::Special:
        break;
    case Object::Material::Transparency::Opaque:
        GameEngine::renderer->AddRenderCallback(this);
        break;
    case Object::Material::Transparency::Transparent:
        GameEngine::renderer->AddTransparentRenderCallback(this);
        break;
    case Object::Material::Transparency::OpaqueNoLight:
        GameEngine::renderer->AddNoLightRenderCallback(this);
        break;
    default:
        Debug::LogWarn("BaseMeshRender: Unknown transparency type in SetRenderCallback");
        break;
    }
}

/// @brief Unsets the render callback based on transparency type when `SetRenderCallback` was called
void Component::BaseMeshRender::UnsetRenderCallback()
{
    switch (this->transparencyCallbackType)
    {
    case Object::Material::Transparency::Special:
        break;
    case Object::Material::Transparency::Opaque:
        GameEngine::renderer->RemoveRenderCallback(this);
        break;
    case Object::Material::Transparency::Transparent:
        GameEngine::renderer->RemoveTransparentRenderCallback(this);
        break;
    case Object::Material::Transparency::OpaqueNoLight:
        GameEngine::renderer->RemoveNoLightRenderCallback(this);
        break;
    default:
        Debug::LogWarn("BaseMeshRender: Unknown transparency type in UnsetRenderCallback");
        break;
    }

    this->transparencyCallbackType = Object::Material::Transparency::Special;
}
