#include "BaseMeshRenderComponent.h"

#include "Engine/Engine.h"
#include "Engine/Lighting.h"

bool Component::BaseMeshRender::IsInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes, glm::vec3 &centroid, double radius)
{
    for (const auto& plane : frustumPlanes) {
        if (glm::dot(glm::vec3(plane), centroid) + plane.w < -radius) {
            return false;
        }
    }

    return true;
}

void Component::BaseMeshRender::Awake()
{
    GameEngine::renderer->AddRenderCallback(this);
}

void Component::BaseMeshRender::OnDestroy()
{
    GameEngine::renderer->RemoveRenderCallback(this);
}

void Component::BaseMeshRender::OnEnable()
{
    GameEngine::renderer->AddRenderCallback(this);
}

void Component::BaseMeshRender::OnDisable()
{
    GameEngine::renderer->RemoveRenderCallback(this);
}

/**
 * @brief Sets up basic rendering uniforms
 * @return Model matrix
 */
glm::mat4 Component::BaseMeshRender::RenderSetTransform()
{
    this->renderShader->Use();

    glm::mat4 model = this->transform->GetMatrixTransform();
    this->renderShader->SetMat4("transform", model);

    return model;
}