#pragma once

#include <Jolt/Jolt.h>

#include "Engine/Physics.h"

#include "Component/BaseComponent.h"
#include "Component/Engine/TransformComponent.h"

namespace Component {
/**
 * @brief Base Collider class
 * @details Handles collision detection for the object
 * @note Updates the transform component during LateUpdate
 */
class BaseCollider : public BaseComponent, public Component::IOffsetUpdatable {
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

    void SetContinuousCollisionDetection(bool enabled);

    void SyncToTransform();
protected:
    std::vector<std::type_index> GetDependencies() const override 
    { return {typeid(Component::Transform)}; }

    static JPH::ShapeRefC ApplyOffset(const JPH::ShapeRefC& ref, glm::vec3 pos, glm::quat rot);
    JPH::BodyID CreateBody(const JPH::ShapeRefC& shape, glm::vec3 pos, glm::quat rot, Physics::Layer layer);
    
    void Awake()     override;
    void OnDestroy() override;
    void OnEnable()  override;
    void OnDisable() override;
    
    virtual void EarlyUpdate() { };
    virtual void LateUpdate();

    void UpdateTransform();

    Component::Transform* transform = nullptr;

    /// @brief Sets the object as static or non-static
    /// @param isStatic 
    /// @note static: transform -> physics
    /// @note non-static: physics -> transform
    void SetStatic(bool isStatic) { this->isStatic = isStatic; }
    bool IsStatic() { return this->isStatic; }

    JPH::BodyID bodyID = JPH::BodyID();

    friend class Object::BaseObject;
private:
    bool isStatic = true;
};
}