#include "StaticCompoundColliderComponent.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>

Component::StaticCompoundCollider::StaticCompoundCollider(Object::BaseObject *owner)
    : BaseCollider(owner)
{
    this->shapeSettings = new JPH::StaticCompoundShapeSettings();
}

Component::StaticCompoundCollider::~StaticCompoundCollider()
{
    if(this->shapeSettings)
        delete this->shapeSettings;
}

void Component::StaticCompoundCollider::AddBox(glm::vec3 size, const glm::vec3 &offset, const glm::quat &rotation)
{
    JPH::Vec3 _offset(size.x, size.y, size.z);
    JPH::Quat _rotation(rotation.w, rotation.x, rotation.y, rotation.z);

    size /= 2.0f;
    size *= this->transform->GetScale();
    JPH::Vec3 _size(size.x, size.y, size.z);
    
    shapeSettings->AddShape(
        _offset, _rotation, 
        new JPH::BoxShapeSettings(_size)
    );
}

void Component::StaticCompoundCollider::AddSphere(float radius, const glm::vec3 &offset, const glm::quat &rotation)
{
    JPH::Vec3 _offset(offset.x, offset.y, offset.z);
    JPH::Quat _rotation(rotation.w, rotation.x, rotation.y, rotation.z);

    radius *= this->transform->GetScale().x;

    shapeSettings->AddShape(
        _offset, _rotation, 
        new JPH::SphereShapeSettings(radius)
    );
}

void Component::StaticCompoundCollider::AddCapsule(float radius, float height, const glm::vec3 &offset, const glm::quat &rotation)
{
    JPH::Vec3 _offset(offset.x, offset.y, offset.z);
    JPH::Quat _rotation(rotation.w, rotation.x, rotation.y, rotation.z);

    glm::vec3 scale = this->transform->GetScale();
    float maxXZScale = std::max(scale.x, scale.z); 
    radius *= maxXZScale;
    height *= scale.y;

    shapeSettings->AddShape(
        _offset, _rotation, 
        new JPH::CapsuleShapeSettings(radius, height)
    );
}

void Component::StaticCompoundCollider::AddConvexMesh(const std::vector<glm::vec3> &vertices, const glm::vec3 &offset, const glm::quat &rotation)
{
    JPH::Vec3 _offset(offset.x, offset.y, offset.z);
    JPH::Quat _rotation(rotation.w, rotation.x, rotation.y, rotation.z);

    glm::vec3 scale = this->transform->GetScale();

    std::vector<JPH::Vec3> joltPoints(vertices.size());
    for(size_t i = 0; i < vertices.size(); ++i){
        glm::vec3 v = vertices[i] * scale;
        joltPoints[i] = JPH::Vec3(v.x, v.y, v.z);
    }

    shapeSettings->AddShape(
        _offset, _rotation, 
        new JPH::ConvexHullShapeSettings(joltPoints.data(), joltPoints.size())
    );
}

void Component::StaticCompoundCollider::Generate(Physics::Layer layer)
{
    JPH::ShapeSettings::ShapeResult result = shapeSettings->Create();
    Debug::Assert(result.IsValid(), "StaticCompoundColliderComponent: Failed to create compound shape");

    JPH::ShapeRefC shape = result.Get();

    glm::vec3 pos = this->transform->GetPos();
    glm::quat rot = this->transform->GetRot();
    this->bodyID = CreateBody(shape, pos, rot, layer);
}


