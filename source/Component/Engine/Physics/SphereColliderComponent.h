#pragma once

#include "Component/Engine/Physics/BaseColliderComponent.h"

namespace Component {
class SphereCollider : public BaseCollider {
public:
    SphereCollider(Object::BaseObject* owner) : BaseCollider(owner) {};
    ~SphereCollider() override = default;
    
    void Generate(float radius, Physics::Layer layer, 
        const glm::vec3 &offset = glm::vec3(0.0f), const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    
protected:

};
}