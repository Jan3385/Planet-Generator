#pragma once

#include "Component/Engine/Physics/BaseColliderComponent.h"

namespace Component {
class BoxCollider : public BaseCollider {
public:
    BoxCollider(Object::BaseObject* owner) : BaseCollider(owner) {};
    ~BoxCollider() override = default;
    
    void Generate(glm::vec3 halfExtents, Physics::Layer layer, 
        const glm::vec3 &offset = glm::vec3(0.0f), const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    
protected:

};
}