#include "CapsuleColliderComponent.h"

#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

/// @brief Generates a capsule collider for the component
/// @param height height of the capsule collider
/// @param radius radius of the capsule collider
/// @param layer layer of the collider
/// @param offset offset of the collider from the object position
/// @param rotation offset rotation of the collider
void Component::CapsuleCollider::Generate(float height, float radius, Physics::Layer layer, const glm::vec3 &offset, const glm::quat &rotation)
{
    if(!this->transform){
        Debug::LogError("CapsuleColliderComponent: Cannot generate collider without a TransformComponent");
        return;
    }

    // scale with object scale
    glm::vec3 scale = this->transform->GetScale();
    float maxXZScale = std::max(scale.x, scale.z); 
    radius *= maxXZScale;
    height *= scale.y;

    JPH::CapsuleShapeSettings settings(radius, height);
    JPH::ShapeSettings::ShapeResult result = settings.Create();
    Debug::Assert(result.IsValid(), "CapsuleColliderComponent: Failed to create capsule shape");

    JPH::ShapeRefC shape = result.Get();

    shape = ApplyOffset(shape, offset, rotation);

    glm::vec3 pos = this->transform->GetPos();
    glm::quat rot = this->transform->GetRot();
    this->bodyID = this->CreateBody(shape, pos, rot, layer);
}