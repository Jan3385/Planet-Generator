#include "Lighting.h"

#include <array>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include "Engine/Engine.h"
#include "Engine/Renderer.h"

Lighting* Lighting::instance = nullptr;

Lighting::Lighting()
{

}

Lighting::~Lighting()
{
    if(instance == this) instance = nullptr;
}

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
    shader->SetVec3("directionalLight.light", this->directionalLightSource.color);
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

void Lighting::RecalculateClosedPointLights(const glm::vec3 &position)
{
    this->closestPLights = this->GetClosestPointLights(position);
}

void Lighting::BindClosestPointLights(uint8_t startIndex, GL::Shader& lightPassShader)
{
    int pointLightCount = 0;
    lightPassShader.Use();
    for (auto* pointLight : closestPLights) {
        if (pointLight != nullptr) {
            pointLight->GetLightData()->Bind(lightPassShader, pointLightCount + startIndex);
            pointLightCount++;
        }
    }
    lightPassShader.SetInt("numPointLights", pointLightCount);
}

std::array<Component::PointLightSource *, Lighting::MAX_EFFECTING_POINT_LIGHTS> Lighting::GetClosestPointLights(const glm::vec3 &position)
{
    using plSource = Component::PointLightSource;

    std::array<plSource*, Lighting::MAX_EFFECTING_POINT_LIGHTS> closestLights = {nullptr};

    // Sort viableLights by distance
    std::sort(this->pointLightSources.begin(), this->pointLightSources.end(), [&position](plSource* a, plSource* b) {
        return glm::distance2(a->GetLightData()->position, position) < glm::distance2(b->GetLightData()->position, position);
    });

    for (size_t i = 0; i < Lighting::MAX_EFFECTING_POINT_LIGHTS; ++i) {
        if(i < this->pointLightSources.size())
            closestLights[i] = this->pointLightSources[i];
        else
            closestLights[i] = nullptr;
    }
    return closestLights;
}

void Lighting::AddPointLightSource(Component::PointLightSource *pointLight)
{
    this->pointLightSources.push_back(pointLight);
}

void Lighting::RemovePointLightSource(Component::PointLightSource *pointLight)
{
    this->pointLightSources.erase(
        std::remove(this->pointLightSources.begin(),
                    this->pointLightSources.end(),
                    pointLight),
        this->pointLightSources.end());
}

void Lighting::InitializeShadowMapping()
{
    Debug::LogSpam("Initializing shadow mapping");
    this->dlShadowFBO = GL::FrameBuffer(GL::DepthBufferMode::Texture);
    dlShadowFBO.DisableDrawRead();
    dlShadowFBO.CompleteSetup();
    dlShadowFBO.UpdateSize(glm::uvec2(SHADOW_MAP_SIZE));

    dlShadowShader = GL::BasicShaderProgram("lighting/directionalShadowShader");

    plShadowShader = GL::ComplexShaderProgram("Point light shader program");
    plShadowShader.AddShader(GL::ShaderType::Vertex, "lighting/pointShadowShader.vert");
    plShadowShader.AddShader(GL::ShaderType::Geometry, "lighting/pointShadowShader.geom");
    plShadowShader.AddShader(GL::ShaderType::Fragment, "lighting/pointShadowShader.frag");
    plShadowShader.Compile();
}

void Lighting::RenderShadowLights()
{
    // setup camera and matrices
    constexpr float OrtographicBoxSize = 35.0f;
    float nearPlane = 1.0f, farPlane = 300.0f;
    glm::mat4 lightProjection = glm::ortho(
        -OrtographicBoxSize, OrtographicBoxSize, 
        -OrtographicBoxSize, OrtographicBoxSize, 
        nearPlane, farPlane
    );

    glm::vec3 cameraPos = GameEngine::currentLevel->GetCamera()->GetPosition();
    glm::mat4 lightView = glm::lookAt(
        cameraPos + (-directionalLightSource.direction * OrtographicBoxSize), 
        cameraPos, 
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    glm::mat4 dirLightSpaceMatrix = lightProjection * lightView;
    GL::Shader::UpdateShaderVariable("mat4 dirLightSpaceMatrix", dirLightSpaceMatrix);

    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    dlShadowFBO.BindShaderFBO();
    glClear(GL_DEPTH_BUFFER_BIT);

    dlShadowShader.Use();

    Renderer::Frustum shadowFrustumPlanes = Renderer::CalculateFrustumPlanes(lightProjection, lightView);
    Renderer::Ins()->RenderShadowMap(dlShadowShader, &shadowFrustumPlanes);

    dlShadowFBO.UnbindShaderFBO();

    for (Component::PointLightSource* pointLight : this->closestPLights) {
        if(pointLight != nullptr)
            pointLight->RenderShadowMap(plShadowShader);
    }

    glm::vec2 screenSize = Renderer::Ins()->GetScreenSize();
    glViewport(0, 0, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));
}

void Lighting::BindShadowMaps(uint8_t startIndex)
{
    glActiveTexture(GL_TEXTURE0 + startIndex);
    glBindTexture(GL_TEXTURE_2D, this->dlShadowFBO.GetDepthStorageID());

    int i = 0;
    for (Component::PointLightSource* pointLight : this->closestPLights) {
        if(pointLight != nullptr)
            pointLight->BindShadowMap(startIndex + 1 + i++);
    }
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

void Lighting::SetDirectionalLightSource(const glm::vec3 &direction, const glm::vec3 &color)
{
    this->directionalLightSource.direction = direction;
    this->directionalLightSource.color = color;
    this->TriggerShaderLightUpdateCallback();
}

void Lighting::TriggerShaderLightUpdateCallback()
{
    GL::Shader::UpdateShaderVariable("vec3 ambientColor", this->ambientColor);
    GL::Shader::UpdateShaderVariable("float ambientIntensity", this->ambientIntensity);

    for (GL::Shader* shader : this->shaderLightUpdateCallbackList) {
        shader->Use();
        shader->SetVec3("directionalLight.direction", this->directionalLightSource.direction);
        shader->SetVec3("directionalLight.light", this->directionalLightSource.color);
    }
}
