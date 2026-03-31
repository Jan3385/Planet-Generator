#include "BaseColliderComponent.h"

#include "Engine/Engine.h"

void Component::BaseCollider::Awake()
{
    transform = GetOwner()->GetComponent<Component::Transform>();
}

void Component::BaseCollider::OnDestroy()
{
    if(!this->bodyID.IsInvalid()){
        GameEngine::physics->RemoveBody(this->bodyID);
        this->bodyID = JPH::BodyID();
    }
}

void Component::BaseCollider::OnEnable()
{
    if(!this->bodyID.IsInvalid())
        GameEngine::physics->EnableBody(this->bodyID);
}

void Component::BaseCollider::OnDisable()
{
    if(!this->bodyID.IsInvalid())
        GameEngine::physics->DisableBody(this->bodyID);
}

void Component::BaseCollider::SyncToTransform()
{
    glm::vec3 pos = this->transform->GetPos();
    glm::quat rot = this->transform->GetRot();

    GameEngine::physics->UpdateBodyTransform(
        this->bodyID,
        JPH::RVec3(pos.x, pos.y, pos.z),
        JPH::Quat(rot.x, rot.y, rot.z, rot.w)
    );
}

void Component::BaseCollider::UpdateTransform()
{
    glm::vec3 pos = GameEngine::physics->GetBodyPosition(this->bodyID);
    glm::quat rot = GameEngine::physics->GetBodyRotation(this->bodyID);

    this->transform->SetPos(pos);
    this->transform->SetRot(rot);
}

void Component::BaseCollider::AddForce(const glm::vec3 &force)
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to add force to an invalid body ID");
    GameEngine::physics->GetBodyInterface().AddForce(this->bodyID, JPH::Vec3(force.x, force.y, force.z));
}

void Component::BaseCollider::AddImpulse(const glm::vec3 &impulse)
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to add impulse to an invalid body ID");
    GameEngine::physics->GetBodyInterface().AddImpulse(this->bodyID, JPH::Vec3(impulse.x, impulse.y, impulse.z));
}

void Component::BaseCollider::AddTorque(const glm::vec3 &torque)
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to add torque to an invalid body ID");
    GameEngine::physics->GetBodyInterface().AddTorque(this->bodyID, JPH::Vec3(torque.x, torque.y, torque.z));
}

void Component::BaseCollider::SetLinearVelocity(const glm::vec3 &velocity)
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to set linear velocity of an invalid body ID");
    GameEngine::physics->GetBodyInterface().SetLinearVelocity(this->bodyID, JPH::Vec3(velocity.x, velocity.y, velocity.z));
}

void Component::BaseCollider::SetAngularVelocity(const glm::vec3 &angularVelocity)
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to set angular velocity of an invalid body ID");
    GameEngine::physics->GetBodyInterface().SetAngularVelocity(
        this->bodyID, 
        JPH::Vec3(angularVelocity.x, angularVelocity.y, angularVelocity.z)
    );
}

glm::vec3 Component::BaseCollider::GetLinearVelocity() const
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to get linear velocity of an invalid body ID");
    const JPH::Vec3 vel = GameEngine::physics->GetBodyInterface().GetLinearVelocity(this->bodyID);

    return glm::vec3(vel.GetX(), vel.GetY(), vel.GetZ());
}

glm::vec3 Component::BaseCollider::GetAngularVelocity() const
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to get angular velocity of an invalid body ID");
    const JPH::Vec3 vel = GameEngine::physics->GetBodyInterface().GetAngularVelocity(this->bodyID);

    return glm::vec3(vel.GetX(), vel.GetY(), vel.GetZ());
}
