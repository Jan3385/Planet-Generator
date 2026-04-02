#pragma once

#include "Component/Engine/Physics/BaseColliderComponent.h"
#include "GLWrapper/Mesh.h"

namespace Component {
/// @brief Mesh collider component
/// @details Generates a mesh collider from a mesh
/// @note Mesh collider can only be *static*
class MeshCollider : public BaseCollider {
public:
    MeshCollider(Object::BaseObject* owner) : BaseCollider(owner) {};
    ~MeshCollider() override = default;
    
    void Generate(const GL::Mesh& mesh, Physics::Layer layer,
        const glm::vec3 &offset = glm::vec3(0.0f), const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    
protected:

};
}