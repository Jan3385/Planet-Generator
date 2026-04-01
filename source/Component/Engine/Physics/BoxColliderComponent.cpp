#include "BoxColliderComponent.h"

#include "Engine/Engine.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

/// @brief Generates a box collider for the component
/// @param halfExtents half of the box collider extents in each direction
/// @param layer layer of the collider
/// @param offset offset of the collider from the object position
/// @param rotation offset rotation of the collider
void Component::BoxCollider::Generate(const glm::vec3 &halfExtents, Physics::Layer layer, 
    const glm::vec3 &offset, const glm::quat &rotation)
{
    JPH::ShapeRefC shape = new JPH::BoxShape(JPH::Vec3(halfExtents.x, halfExtents.y, halfExtents.z), 0.0f);

    shape = ApplyOffset(shape, offset, rotation);

    glm::vec3 pos = this->transform->GetPos();
    glm::quat rot = this->transform->GetRot();
    this->bodyID = this->CreateBody(shape, pos, rot, layer);
}
