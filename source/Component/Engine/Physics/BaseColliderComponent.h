#pragma once

#include <Jolt/Jolt.h>

#include "Engine/Physics.h"

#include "Component/BaseComponent.h"
#include "Component/Engine/TransformComponent.h"

namespace Component {
/**
 * @brief Base Collider class
 * @details Handles collision detection for the object
 */
class BaseCollider : public BaseComponent {
public:
    BaseCollider(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~BaseCollider() override = default;

    void SetTransform(Component::Transform* newTransform) { transform = newTransform; }

    void AddForce(const glm::vec3 &force);
    void AddImpulse(const glm::vec3 &impulse);
    void AddTorque(const glm::vec3 &torque);
    void SetLinearVelocity(const glm::vec3 &velocity);
    void SetAngularVelocity(const glm::vec3 &angularVelocity);
    
    glm::vec3 GetLinearVelocity() const;
    glm::vec3 GetAngularVelocity() const;

    void SyncToTransform();
    void UpdateTransform(); //TODO: update transform during LateUpdate
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }

    void Awake()     override;
    void OnDestroy() override;
    void OnEnable()  override;
    void OnDisable() override;

    Component::Transform* transform = nullptr;

    JPH::BodyID bodyID = JPH::BodyID();
};
}