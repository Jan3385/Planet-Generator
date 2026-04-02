#include "SphereColliderComponent.h"

#include <Jolt/Physics/Collision/Shape/SphereShape.h>

/// @brief Generates a sphere collider for the component
/// @param radius radius of the sphere collider
/// @param layer layer of the collider
/// @param offset offset of the collider from the object position
/// @param rotation offset rotation of the collider
void Component::SphereCollider::Generate(float radius, Physics::Layer layer, const glm::vec3 &offset, const glm::quat &rotation)
{
    if(!this->transform){
        Debug::LogError("SphereColliderComponent: Cannot generate collider without a TransformComponent");
        return;
    }

    // scale with object scale
    glm::vec3 scale = this->transform->GetScale();
    float maxScale = std::max(scale.x, std::max(scale.y, scale.z)); 
    radius *= maxScale;

    JPH::SphereShapeSettings settings(radius);
    JPH::ShapeSettings::ShapeResult result = settings.Create();
    Debug::Assert(result.IsValid(), "SphereColliderComponent: Failed to create sphere shape");

    JPH::ShapeRefC shape = result.Get();

    shape = ApplyOffset(shape, offset, rotation);

    glm::vec3 pos = this->transform->GetPos();
    glm::quat rot = this->transform->GetRot();
    this->bodyID = this->CreateBody(shape, pos, rot, layer);
}