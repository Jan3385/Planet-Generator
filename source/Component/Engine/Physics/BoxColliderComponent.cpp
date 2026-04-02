#include "BoxColliderComponent.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>

/// @brief Generates a box collider for the component
/// @param size size of the box collider in each direction
/// @param layer layer of the collider
/// @param offset offset of the collider from the object position
/// @param rotation offset rotation of the collider
void Component::BoxCollider::Generate(glm::vec3 size, Physics::Layer layer, 
    const glm::vec3 &offset, const glm::quat &rotation)
{
    if(!this->transform){
        Debug::LogError("BoxColliderComponent: Cannot generate collider without a TransformComponent");
        return;
    }

    size /= 2.0f; // Jolt needs half of the size

    // scale with object scale 
    size *= this->transform->GetScale();

    JPH::BoxShapeSettings settings(JPH::Vec3(size.x, size.y, size.z), 0.0f);
    JPH::ShapeSettings::ShapeResult result = settings.Create();
    Debug::Assert(result.IsValid(), "BoxColliderComponent: Failed to create box shape");

    JPH::ShapeRefC shape = result.Get();

    shape = ApplyOffset(shape, offset, rotation);

    glm::vec3 pos = this->transform->GetPos();
    glm::quat rot = this->transform->GetRot();
    this->bodyID = this->CreateBody(shape, pos, rot, layer);
}
