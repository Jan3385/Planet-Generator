#include "GravityForceComponent.h"

#include "Object/BaseObject.h"

#include "Component/Game/Planet/GravityGeneratorComponent.h"

void Component::GravityForce::Awake()
{
    this->transform = this->GetOwner()->GetComponent<Component::Transform>();
    this->collider = this->GetOwner()->GetComponent<Component::BaseCollider>();
}

void Component::GravityForce::Update()
{
}

void Component::GravityForce::FixedUpdate()
{
    glm::vec3 totalGravity = Component::GravityGenerator::GetGravityAt(this->transform->GetPos());
    this->collider->AddForce(totalGravity);
}
