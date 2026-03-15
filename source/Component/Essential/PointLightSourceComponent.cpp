#include "PointLightSourceComponent.h"

#include "Object/BaseObject.h"
#include "Engine/Engine.h"
#include "Debug/Logger.h"

constexpr unsigned int SHADOW_MAP_SIZE = 512;

Component::PointLightSource* Component::PointLightSource::SetLightData(const PointLightData &newData)
{
    this->data.color = newData.color * newData.intensity;
    this->data.constant = newData.constant;
    this->data.linear = newData.linear;
    this->data.quadratic = newData.quadratic;
    return this;
}

Component::PointLightSource* Component::PointLightSource::SetLightData(glm::vec3 color, float linear, float quadratic, float radius)
{
    this->data.color = color;
    this->data.constant = 1.0f;
    this->data.linear = linear;
    this->data.quadratic = quadratic;

    return this;
}

void Component::PointLightSource::RenderShadowMap(GL::Shader &shadowShader)
{
    if(!this->shadowFBO.Initialized()) return;

    glm::vec2 textureSize = this->shadowFBO.GetSize();
    glViewport(0, 0, textureSize.x, textureSize.y);

    float aspectRatio = textureSize.x / textureSize.y;
    constexpr float nearPlane = 0.1f;
    constexpr float farPlane = POINT_LIGHT_FAR_PLANE;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspectRatio, nearPlane, farPlane);

    glm::mat4 shadowTransforms[6];
    const glm::vec3& pos = this->data.position;
    shadowTransforms[0] = shadowProj * glm::lookAt(pos, pos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    shadowTransforms[1] = shadowProj * glm::lookAt(pos, pos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    shadowTransforms[2] = shadowProj * glm::lookAt(pos, pos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f));
    shadowTransforms[3] = shadowProj * glm::lookAt(pos, pos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f));
    shadowTransforms[4] = shadowProj * glm::lookAt(pos, pos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    shadowTransforms[5] = shadowProj * glm::lookAt(pos, pos + glm::vec3( 1.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f));

    this->shadowFBO.BindShaderFBO();
    glClear(GL_DEPTH_BUFFER_BIT);
    shadowShader.Use();

    shadowShader.SetVec3("pointLightPos", pos);
    shadowShader.SetFloat("farPlane", POINT_LIGHT_FAR_PLANE);

    for (uint8_t i = 0; i < 6; i++)
        shadowShader.SetMat4(std::format("shadowMatrices[{}]", i), shadowTransforms[i]);
    
    GameEngine::renderer->RenderShadowMap(shadowShader, nullptr);

    this->shadowFBO.UnbindShaderFBO();
}

void Component::PointLightSource::BindShadowMap(uint8_t textureIndex) const
{
    glActiveTexture(GL_TEXTURE0 + textureIndex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->shadowFBO.GetDepthStorageID());
}

void Component::PointLightSource::Awake()
{
    if(!this->transform)
        this->transform = this->GetOwner()->GetComponent<Component::Transform>();

    this->data.position = this->transform->GetPos();

    if(!this->shadowFBO.Initialized()){
        this->shadowFBO = GL::FrameBuffer(GL::DepthBufferMode::Cubemap, glm::uvec2(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE));
        this->shadowFBO.CompleteSetup();
    }

    GameEngine::lighting->AddPointLightSource(this);
}

void Component::PointLightSource::OnDestroy()
{
    GameEngine::lighting->RemovePointLightSource(this);
}

void Component::PointLightSource::OnEnable()
{
    GameEngine::lighting->AddPointLightSource(this);
}

void Component::PointLightSource::OnDisable()
{
    GameEngine::lighting->RemovePointLightSource(this);
}

void Component::PointLightSource::Update()
{
    if(!this->transform) return;

    this->data.position = this->transform->GetPos();
}
