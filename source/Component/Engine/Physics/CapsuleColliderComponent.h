#pragma once

#include "Component/Engine/Physics/BaseColliderComponent.h"

namespace Component {
class CapsuleCollider : public BaseCollider {
public:
    CapsuleCollider(Object::BaseObject* owner) : BaseCollider(owner) {};
    ~CapsuleCollider() override = default;
    
    void Generate(float height, float radius, Physics::Layer layer, 
        const glm::vec3 &offset = glm::vec3(0.0f), const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    
protected:

};
}