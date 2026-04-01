#include "BoxColliderComponent.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>

/// @brief Generates a box collider for the component
/// @param halfExtents half of the box collider extents in each direction
/// @param layer layer of the collider
/// @param offset offset of the collider from the object position
/// @param rotation offset rotation of the collider
void Component::BoxCollider::Generate(glm::vec3 halfExtents, Physics::Layer layer, 
    const glm::vec3 &offset, const glm::quat &rotation)
{
    if(!this->transform){
        Debug::LogError("BoxColliderComponent: Cannot generate collider without a TransformComponent");
        return;
    }

    // scale with object scale 
    halfExtents.x *= this->transform->GetScale().x;
    halfExtents.y *= this->transform->GetScale().y;
    halfExtents.z *= this->transform->GetScale().z;

    JPH::BoxShapeSettings settings(JPH::Vec3(halfExtents.x, halfExtents.y, halfExtents.z), 0.0f);
    JPH::ShapeSettings::ShapeResult result = settings.Create();
    Debug::Assert(result.IsValid(), "BoxColliderComponent: Failed to create box shape");

    JPH::ShapeRefC shape = result.Get();

    shape = ApplyOffset(shape, offset, rotation);

    glm::vec3 pos = this->transform->GetPos();
    glm::quat rot = this->transform->GetRot();
    this->bodyID = this->CreateBody(shape, pos, rot, layer);
}
