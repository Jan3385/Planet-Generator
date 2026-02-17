#include "Lighting.h"

#include <array>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

void Lighting::SetAmbientColor(const glm::vec3 &color)
{
    if(this->ambientColor == color) return;

    this->ambientColor = color;
    this->TriggerShaderLightUpdateCallback();
}

void Lighting::SetAmbientIntensity(float intensity)
{
    if(this->ambientIntensity == intensity) return;

    this->ambientIntensity = intensity;
    this->TriggerShaderLightUpdateCallback();
}

/**
 * @brief Registers a shader to receive light change updates
 * @param shader Pointer to the shader to register
 */
void Lighting::RegisterShaderLightUpdateCallback(GL::Shader *shader)
{
    //TODO: something different like with variables this is too silly
    this->shaderLightUpdateCallbackList.push_back(shader);
    shader->Use();
    shader->SetVec3("directionalLight.direction", this->directionalLightSource.direction);
    shader->SetVec3("directionalLight.diffuse", this->directionalLightSource.diffuse);
    shader->SetVec3("directionalLight.specular", this->directionalLightSource.specular);
}

/**
 * @brief Unregisters a shader from receiving light change updates
 * @param shader Pointer to the shader to unregister
 */
void Lighting::UnregisterShaderLightUpdateCallback(GL::Shader *shader)
{
    this->shaderLightUpdateCallbackList.erase(
        std::remove(this->shaderLightUpdateCallbackList.begin(),
                    this->shaderLightUpdateCallbackList.end(),
                    shader),
        this->shaderLightUpdateCallbackList.end());
}

std::array<Lighting::PointLightSource *, Lighting::MAX_EFFECTING_POINT_LIGHTS> Lighting::GetClosestPointLights(const glm::vec3 &position)
{
    std::vector<Lighting::PointLightSource*> viableLights;
    for (Lighting::PointLightSource* light : this->pointLightSources) {
        viableLights.push_back(light);
    }
    std::array<Lighting::PointLightSource *, Lighting::MAX_EFFECTING_POINT_LIGHTS> closestLights = {nullptr};

    // Sort viableLights by distance
    std::sort(viableLights.begin(), viableLights.end(), [&position](Lighting::PointLightSource* a, Lighting::PointLightSource* b) {
        return glm::distance2(a->position, position) < glm::distance2(b->position, position);
    });

    for (size_t i = 0; i < Lighting::MAX_EFFECTING_POINT_LIGHTS && i < viableLights.size(); ++i) {
        closestLights[i] = viableLights[i];
    }
    return closestLights;
}

void Lighting::AddPointLightSource(PointLightSource *pointLight)
{
    this->pointLightSources.push_back(pointLight);
}

void Lighting::RemovePointLightSource(PointLightSource *pointLight)
{
    this->pointLightSources.erase(
        std::remove(this->pointLightSources.begin(),
                    this->pointLightSources.end(),
                    pointLight),
        this->pointLightSources.end());
}

void Lighting::SetDirectionalLightSourceDirection(const glm::vec3 &direction)
{
    this->directionalLightSource.direction = direction;
    this->TriggerShaderLightUpdateCallback();
}

void Lighting::SetDirectionalLightSource(const DirectionLightSource &directionalLight)
{
    this->directionalLightSource = directionalLight;
    this->TriggerShaderLightUpdateCallback();
}

void Lighting::SetDirectionalLightSource(const glm::vec3 &direction, const glm::vec3 &diffuse, const glm::vec3 &specular)
{
    this->directionalLightSource.direction = direction;
    this->directionalLightSource.diffuse = diffuse;
    this->directionalLightSource.specular = specular;
    this->TriggerShaderLightUpdateCallback();
}

void Lighting::TriggerShaderLightUpdateCallback()
{
    GL::Shader::UpdateShaderVariable("vec3 ambientColor", this->ambientColor);
    GL::Shader::UpdateShaderVariable("float ambientIntensity", this->ambientIntensity);

    for (GL::Shader* shader : this->shaderLightUpdateCallbackList) {
        shader->Use();
        shader->SetVec3("directionalLight.direction", this->directionalLightSource.direction);
        shader->SetVec3("directionalLight.diffuse", this->directionalLightSource.diffuse);
        shader->SetVec3("directionalLight.specular", this->directionalLightSource.specular);
    }
}
