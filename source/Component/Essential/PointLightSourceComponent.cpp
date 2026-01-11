#include "PointLightSourceComponent.h"

#include "Object/BaseObject.h"
#include "Engine/Engine.h"
#include "Debug/Logger.h"

void Component::PointLightSource::SetLightData(const PointLightData &newData)
{
    this->data.color = newData.color;
    this->data.intensity = newData.intensity;
    this->data.radius = newData.radius;
}

void Component::PointLightSource::SetLightData(glm::vec3 color, float intensity, float radius)
{
    this->data.color = color;
    this->data.intensity = intensity;
    this->data.radius = radius;
}

Component::PointLightSource::PointLightData Component::PointLightSource::GetLightData() const
{
    PointLightData lightData;
    lightData.color = this->data.color;
    lightData.intensity = this->data.intensity;
    lightData.radius = this->data.radius;
    return lightData;
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
