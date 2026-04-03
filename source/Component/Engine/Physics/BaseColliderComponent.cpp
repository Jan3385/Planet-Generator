#include "BaseColliderComponent.h"

#include "Engine/Physics.h"

#include "Object/BaseObject.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

JPH::ShapeRefC Component::BaseCollider::ApplyOffset(const JPH::ShapeRefC& ref, glm::vec3 pos, glm::quat rot)
{
    if(pos != glm::vec3(0.0f) || rot != glm::quat(1.0f, 0.0f, 0.0f, 0.0f)){
        JPH::RotatedTranslatedShapeSettings offsetShape(
            JPH::RVec3(pos.x, pos.y, pos.z),
            JPH::Quat(rot.x, rot.y, rot.z, rot.w),
            ref
        );
        return offsetShape.Create().Get();
    }

    return ref;
}

JPH::BodyID Component::BaseCollider::CreateBody(const JPH::ShapeRefC& shape, glm::vec3 pos, glm::quat rot, Physics::Layer layer)
{
    JPH::EMotionType motionType = Physics::GetMotionType(layer);

    rot = glm::normalize(rot);
    if(rot == glm::quat(0.0f, 0.0f, 0.0f, 0.0f)) rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    JPH::BodyCreationSettings settings(
        shape, 
        JPH::RVec3(pos.x, pos.y, pos.z), 
        JPH::Quat(rot.x, rot.y, rot.z, rot.w), 
        motionType, 
        static_cast<JPH::ObjectLayer>(layer)
    );

    settings.mUserData = reinterpret_cast<JPH::uint64>(this); // for collision callbacks
    //settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateMassAndInertia;
    
    this->SetStatic(motionType != JPH::EMotionType::Dynamic);
    this->motionType = motionType;

    return Physics::Ins()->CreateBody(settings, this);
}

void Component::BaseCollider::Awake()
{
    transform = GetOwner()->GetComponent<Component::Transform>();
}

void Component::BaseCollider::OnDestroy()
{
    if(!this->bodyID.IsInvalid()){
        Physics::Ins()->RemoveBody(this->bodyID);
        this->bodyID = JPH::BodyID();
    }
}

void Component::BaseCollider::OnEnable()
{
    if(!this->bodyID.IsInvalid())
        Physics::Ins()->EnableBody(this->bodyID);
}

void Component::BaseCollider::OnDisable()
{
    if(!this->bodyID.IsInvalid())
        Physics::Ins()->DisableBody(this->bodyID);
}

void Component::BaseCollider::SyncToTransform()
{
    if(this->bodyID.IsInvalid()) return;

    glm::vec3 pos = this->transform->GetPos();
    glm::quat rot = this->transform->GetRot();

    Physics::Ins()->UpdateBodyTransform(
        this->bodyID,
        JPH::RVec3(pos.x, pos.y, pos.z),
        JPH::Quat(rot.x, rot.y, rot.z, rot.w),
        this->motionType == JPH::EMotionType::Kinematic
    );
}

void Component::BaseCollider::LateUpdate()
{
    if(this->bodyID.IsInvalid()) return;

    if(this->isStatic)
        this->SyncToTransform();
    else
        this->UpdateTransform();
}

void Component::BaseCollider::UpdateTransform()
{
    if(this->bodyID.IsInvalid()) return;

    glm::vec3 pos = Physics::Ins()->GetBodyPosition(this->bodyID);
    glm::quat rot = Physics::Ins()->GetBodyRotation(this->bodyID);

    this->transform->SetPos(pos);
    this->transform->SetRot(rot);
}

void Component::BaseCollider::AddForce(const glm::vec3 &force)
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to add force to an invalid body ID");
    Physics::Ins()->GetBodyInterface().AddForce(this->bodyID, JPH::Vec3(force.x, force.y, force.z));
}

void Component::BaseCollider::AddImpulse(const glm::vec3 &impulse)
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to add impulse to an invalid body ID");
    Physics::Ins()->GetBodyInterface().AddImpulse(this->bodyID, JPH::Vec3(impulse.x, impulse.y, impulse.z));
}

void Component::BaseCollider::AddTorque(const glm::vec3 &torque)
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to add torque to an invalid body ID");
    Physics::Ins()->GetBodyInterface().AddTorque(this->bodyID, JPH::Vec3(torque.x, torque.y, torque.z));
}

void Component::BaseCollider::SetLinearVelocity(const glm::vec3 &velocity)
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to set linear velocity of an invalid body ID");
    Physics::Ins()->GetBodyInterface().SetLinearVelocity(this->bodyID, JPH::Vec3(velocity.x, velocity.y, velocity.z));
}

void Component::BaseCollider::SetAngularVelocity(const glm::vec3 &angularVelocity)
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to set angular velocity of an invalid body ID");
    Physics::Ins()->GetBodyInterface().SetAngularVelocity(
        this->bodyID, 
        JPH::Vec3(angularVelocity.x, angularVelocity.y, angularVelocity.z)
    );
}

glm::vec3 Component::BaseCollider::GetLinearVelocity() const
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to get linear velocity of an invalid body ID");
    const JPH::Vec3 vel = Physics::Ins()->GetBodyInterface().GetLinearVelocity(this->bodyID);

    return glm::vec3(vel.GetX(), vel.GetY(), vel.GetZ());
}

glm::vec3 Component::BaseCollider::GetAngularVelocity() const
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to get angular velocity of an invalid body ID");
    const JPH::Vec3 vel = Physics::Ins()->GetBodyInterface().GetAngularVelocity(this->bodyID);

    return glm::vec3(vel.GetX(), vel.GetY(), vel.GetZ());
}

void Component::BaseCollider::SetContinuousCollisionDetection(bool enabled)
{
    Debug::AssertNot(this->bodyID.IsInvalid(), "[BaseCollider] Attempted to set CCD of an invalid body ID");

    Physics::Ins()->GetBodyInterface().SetMotionQuality(
        this->bodyID, 
        enabled ? JPH::EMotionQuality::LinearCast : JPH::EMotionQuality::Discrete
    );
}
