#pragma once

#include <vector>

#include "GLWrapper/Shader.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Lighting {
public:
    Lighting() = default;
    ~Lighting() = default;

    void SetAmbientColor(const glm::vec3& color);
    glm::vec3 GetAmbientColor() const { return this->ambientColor; }
    void SetAmbientIntensity(float intensity);
    float GetAmbientIntensity() const { return this->ambientIntensity; }

    void RegisterShaderLightUpdateCallback(GL::Shader* shader);
    void UnregisterShaderLightUpdateCallback(GL::Shader* shader);
private:
    void TriggerShaderLightUpdateCallback();
    std::vector<GL::Shader*> shaderLightUpdateCallbackList;

    glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float ambientIntensity = 0.2f;
};