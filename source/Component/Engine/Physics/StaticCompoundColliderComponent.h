#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>

#include "Component/Engine/Physics/BaseColliderComponent.h"

#include "GLWrapper/mesh.h"

namespace Component {
class StaticCompoundCollider : public BaseCollider {
public:
    StaticCompoundCollider(Object::BaseObject* owner);
    ~StaticCompoundCollider() override;
    
    void AddBox(glm::vec3 size,                                 const glm::vec3 &offset = glm::vec3(0.0f), const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    void AddSphere(float radius,                                const glm::vec3 &offset = glm::vec3(0.0f), const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    void AddCapsule(float radius, float height,                 const glm::vec3 &offset = glm::vec3(0.0f), const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    void AddConvexMesh(const std::vector<glm::vec3> &vertices,  const glm::vec3 &offset = glm::vec3(0.0f), const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    void Generate(Physics::Layer layer);
protected:
    JPH::StaticCompoundShapeSettings *shapeSettings = nullptr;
};
}