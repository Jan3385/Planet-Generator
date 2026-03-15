#pragma once

#include <vector>

#include "GLWrapper/Shader.h"
#include "GLWrapper/BasicShaderProgram.h"
#include "GLWrapper/ComplexShaderProgram.h"
#include "GLWrapper/FrameBuffer.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Component/Essential/PointLightSourceComponent.h"

class Lighting {
public:
    static constexpr bool LOW_POLY_LIGHTING_FEEL = false;
    static constexpr int MAX_EFFECTING_POINT_LIGHTS = 16;

    struct DirectionLightSource {
        glm::vec3 direction;
        glm::vec3 color;
    };

    Lighting();
    ~Lighting() = default;

    void SetAmbientColor(const glm::vec3& color);
    glm::vec3 GetAmbientColor() const { return this->ambientColor; }
    void SetAmbientIntensity(float intensity);
    float GetAmbientIntensity() const { return this->ambientIntensity; }

    void RegisterShaderLightUpdateCallback(GL::Shader* shader);
    void UnregisterShaderLightUpdateCallback(GL::Shader* shader);

    void RecalculateClosedPointLights(const glm::vec3& position);
    void BindClosestPointLights(uint8_t startIndex, GL::Shader& lightPassShader);
    void BindShadowMaps(uint8_t startIndex);
    std::array<Component::PointLightSource*, MAX_EFFECTING_POINT_LIGHTS> GetClosestPointLights(const glm::vec3& position);

    void AddPointLightSource(Component::PointLightSource* pointLight);
    void RemovePointLightSource(Component::PointLightSource* pointLight);

    void InitializeShadowMapping();
    void RenderShadowLights();

    void SetDirectionalLightSourceDirection(const glm::vec3& direction);
    void SetDirectionalLightSource(const DirectionLightSource& directionalLight);
    void SetDirectionalLightSource(const glm::vec3& direction,
                                   const glm::vec3& color);  
private:
    std::array<Component::PointLightSource*, MAX_EFFECTING_POINT_LIGHTS> closestPLights;

    GL::BasicShaderProgram dlShadowShader;
    GL::ComplexShaderProgram plShadowShader;

    std::vector<Component::PointLightSource*> pointLightSources;

    constexpr static unsigned int SHADOW_MAP_SIZE = 2048;
    DirectionLightSource directionalLightSource;
    glm::mat4 lightSpaceMatrix;
    GL::FrameBuffer dlShadowFBO;

    void TriggerShaderLightUpdateCallback();
    std::vector<GL::Shader*> shaderLightUpdateCallbackList;

    glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float ambientIntensity = 0.2f;
};