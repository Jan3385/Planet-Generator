#include "PointLightSourceComponent.h"

#include "Object/BaseObject.h"
#include "Engine/Engine.h"
#include "Debug/Logger.h"

Component::PointLightSource* Component::PointLightSource::SetLightData(const PointLightData &newData)
{
    this->data.diffuse = newData.color * newData.intensity;
    this->data.specular = newData.color * newData.intensity;
    this->data.constant = newData.constant;
    this->data.linear = newData.linear;
    this->data.quadratic = newData.quadratic;
    this->data.radius = newData.radius;
    return this;
}

Component::PointLightSource* Component::PointLightSource::SetLightData(glm::vec3 color, float diffuseIntensity, float specularIntensity, float linear, float quadratic, float radius)
{
    this->data.diffuse = color * diffuseIntensity;
    this->data.specular = color * specularIntensity;
    this->data.constant = 1.0f;
    this->data.linear = linear;
    this->data.quadratic = quadratic;
    this->data.radius = radius;

    return this;
}

void Component::PointLightSource::Awake()
{
    if(!this->transform)
        this->transform = this->GetOwner()->GetComponent<Component::Transform>();

    this->data.position = this->transform->GetPos();

    GameEngine::lighting->AddPointLightSource(&this->data);
}

void Component::PointLightSource::OnDestroy()
{
    GameEngine::lighting->RemovePointLightSource(&this->data);
}

void Component::PointLightSource::OnEnable()
{
    GameEngine::lighting->AddPointLightSource(&this->data);
}

void Component::PointLightSource::OnDisable()
{
    GameEngine::lighting->RemovePointLightSource(&this->data);
}

void Component::PointLightSource::Update()
{
    if(!this->transform) return;

    this->data.position = this->transform->GetPos();
}
