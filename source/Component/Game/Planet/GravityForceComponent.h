#pragma once

#include "Component/BaseComponent.h"
#include "Component/Engine/TransformComponent.h"
#include "Component/Engine/Physics/BaseColliderComponent.h"

namespace Component {
/**
 * 
 */
class GravityForce : public BaseComponent, public Component::IUpdatable{
public:
    GravityForce(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~GravityForce() override = default;

    GravityForce* SetTransform(Component::Transform* transform) {
        this->transform = transform;
        return this;
    }

    GravityForce* SetCollider(Component::BaseCollider* collider) {
        this->collider = collider;
        return this;
    }
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform), typeid(Component::BaseCollider) }; }
private:
    Component::Transform* transform = nullptr;
    Component::BaseCollider* collider = nullptr;

    void Awake() override;  

    void Update() override;
    void FixedUpdate() override;

    friend class Object::BaseObject;
};
}