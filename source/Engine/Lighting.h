#pragma once

#include <vector>

#include "GLWrapper/Shader.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Lighting {
public:
    static constexpr bool LOW_POLY_LIGHTING_FEEL = false;
    static constexpr int MAX_EFFECTING_POINT_LIGHTS = 4;

    struct PointLightSource {
        glm::vec3 position;

        glm::vec3 diffuse;
        glm::vec3 specular;

        float constant;
        float linear;
        float quadratic;

        float radius;

        void Bind(GL::Shader& shader, int index) const{
            std::string baseName = "pointLights[" + std::to_string(index) + "]";
            shader.SetVec3(baseName + ".position", this->position);
            shader.SetVec3(baseName + ".diffuse", this->diffuse);
            shader.SetVec3(baseName + ".specular", this->specular);
            shader.SetFloat(baseName + ".constant", this->constant);
            shader.SetFloat(baseName + ".linear", this->linear);
            shader.SetFloat(baseName + ".quadratic", this->quadratic);
        };
    };

    struct DirectionLightSource {
        glm::vec3 direction;

        glm::vec3 diffuse;
        glm::vec3 specular;
    };

    Lighting() = default;
    ~Lighting() = default;

    void SetAmbientColor(const glm::vec3& color);
    glm::vec3 GetAmbientColor() const { return this->ambientColor; }
    void SetAmbientIntensity(float intensity);
    float GetAmbientIntensity() const { return this->ambientIntensity; }

    void RegisterShaderLightUpdateCallback(GL::Shader* shader);
    void UnregisterShaderLightUpdateCallback(GL::Shader* shader);

    std::array<PointLightSource*, MAX_EFFECTING_POINT_LIGHTS> GetClosestPointLights(const glm::vec3& position);

    void AddPointLightSource(PointLightSource* pointLight);
    void RemovePointLightSource(PointLightSource* pointLight);

    void SetDirectionalLightSource(const DirectionLightSource& directionalLight);
    void SetDirectionalLightSource(const glm::vec3& direction,
                                   const glm::vec3& diffuse,
                                   const glm::vec3& specular);  
private:
    std::vector<PointLightSource*> pointLightSources;
    DirectionLightSource directionalLightSource;

    void TriggerShaderLightUpdateCallback();
    std::vector<GL::Shader*> shaderLightUpdateCallbackList;

    glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float ambientIntensity = 0.2f;
};