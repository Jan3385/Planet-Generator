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

    if(offset != glm::vec3(0.0f) || rotation != glm::quat(1.0f, 0.0f, 0.0f, 0.0f)){
        JPH::RotatedTranslatedShapeSettings offsetShape(
            JPH::RVec3(offset.x, offset.y, offset.z),
            JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
            shape
        );
        shape = offsetShape.Create().Get();
    }

    glm::vec3 pos = this->transform->GetPos();
    glm::quat rot = this->transform->GetRot();
    JPH::BodyCreationSettings settings(
        shape, 
        JPH::RVec3(pos.x, pos.y, pos.z), 
        JPH::Quat(rot.x, rot.y, rot.z, rot.w), 
        Physics::GetMotionType(layer), 
        static_cast<JPH::ObjectLayer>(layer)
    );

    this->bodyID = GameEngine::physics->CreateBody(settings);
}
