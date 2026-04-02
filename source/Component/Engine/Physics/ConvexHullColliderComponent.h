#pragma once

#include "Component/Engine/Physics/BaseColliderComponent.h"
#include "GLWrapper/Mesh.h"

namespace Component {

/// @brief Convex hull collider component
/// @details Generates a convex hull collider from a set of points or a mesh
/// @note Convex hull can be static, non-static and kinematic
class ConvexHullCollider : public BaseCollider {
public:
    ConvexHullCollider(Object::BaseObject* owner) : BaseCollider(owner) {};
    ~ConvexHullCollider() override = default;
    
    void Generate(std::vector<glm::vec3> points, Physics::Layer layer,
        const glm::vec3 &offset = glm::vec3(0.0f), const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    
    void Generate(std::vector<JPH::Vec3> points, Physics::Layer layer,
        const glm::vec3 &offset = glm::vec3(0.0f), const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    void Generate(const GL::Mesh& mesh, Physics::Layer layer,
        const glm::vec3 &offset = glm::vec3(0.0f), const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    
protected:

};
}