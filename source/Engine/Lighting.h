#pragma once

#include <vector>

#include "GLWrapper/Shader.h"
#include "GLWrapper/BasicShaderProgram.h"
#include "GLWrapper/ComplexShaderProgram.h"
#include "GLWrapper/FrameBuffer.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Lighting {
public:
    static constexpr bool LOW_POLY_LIGHTING_FEEL = false;
    static constexpr int MAX_EFFECTING_POINT_LIGHTS = 16;

    struct PointLightSourceData {
        glm::vec3 position;

        glm::vec3 color;

        float constant;
        float linear;
        float quadratic;

        void Bind(GL::Shader& shader, int index) const{
            std::string baseName = "pointLights[" + std::to_string(index) + "]";
            shader.SetVec3(baseName + ".position", this->position);
            shader.SetVec3(baseName + ".light", this->color);
            shader.SetFloat(baseName + ".constant", this->constant);
            shader.SetFloat(baseName + ".linear", this->linear);
            shader.SetFloat(baseName + ".quadratic", this->quadratic);
        };
    };

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

    std::array<PointLightSourceData*, MAX_EFFECTING_POINT_LIGHTS> GetClosestPointLights(const glm::vec3& position);

    void AddPointLightSource(PointLightSourceData* pointLight);
    void RemovePointLightSource(PointLightSourceData* pointLight);

    void InitializeShadowMapping();
    void RenderShadowDirectionalLight();
    GLuint GetShadowMapTextureID() const { return this->dlShadowFBO.GetDepthStorageID(); }

    void SetDirectionalLightSourceDirection(const glm::vec3& direction);
    void SetDirectionalLightSource(const DirectionLightSource& directionalLight);
    void SetDirectionalLightSource(const glm::vec3& direction,
                                   const glm::vec3& color);  
private:
    GL::BasicShaderProgram dlShadowShader;
    GL::ComplexShaderProgram plShadowShader;

    std::vector<PointLightSourceData*> pointLightSources;

    constexpr static unsigned int SHADOW_MAP_SIZE = 2048;
    DirectionLightSource directionalLightSource;
    glm::mat4 lightSpaceMatrix;
    GL::FrameBuffer dlShadowFBO;

    void TriggerShaderLightUpdateCallback();
    std::vector<GL::Shader*> shaderLightUpdateCallbackList;

    glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float ambientIntensity = 0.2f;
};