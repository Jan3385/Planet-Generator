#include "PointLightSourceComponent.h"

#include "Object/BaseObject.h"
#include "Engine/Engine.h"
#include "Debug/Logger.h"

void Component::PointLightSource::SetLightData(const PointLightData &newData)
{
    this->data.diffuse = newData.color * newData.intensity;
    this->data.specular = newData.color * newData.intensity;
    this->data.radius = newData.radius;
}

void Component::PointLightSource::SetLightData(glm::vec3 color, float diffuseIntensity, float specularIntensity, float radius)
{
    this->data.diffuse = color * diffuseIntensity;
    this->data.specular = color * specularIntensity;
    this->data.radius = radius;
}

void Component::PointLightSource::Awake()
{
    this->transform = this->GetOwner()->GetComponent<Component::Transform>();
    if(!this->transform) {
        Debug::LogFatal("PointLightSource component requires a Transform component to function properly!");
        return;
    }

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
