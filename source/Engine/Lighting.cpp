#include "Lighting.h"

#include <algorithm>

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
 * @note The shader is expected to have uniform variables called "Vec3 ambientColor" and "float ambientIntensity"
 */
void Lighting::RegisterShaderLightUpdateCallback(GL::Shader *shader)
{
    this->shaderLightUpdateCallbackList.push_back(shader);
    shader->Use();
    shader->SetVec3("ambientColor", this->ambientColor);
    shader->SetFloat("ambientIntensity", this->ambientIntensity);
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

void Lighting::TriggerShaderLightUpdateCallback()
{
    for (GL::Shader* shader : this->shaderLightUpdateCallbackList) {
        shader->Use();
        shader->SetVec3("ambientColor", this->ambientColor);
        shader->SetFloat("ambientIntensity", this->ambientIntensity);
    }
}
