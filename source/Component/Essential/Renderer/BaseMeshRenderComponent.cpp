#include "BaseMeshRenderComponent.h"

#include "Engine/Engine.h"
#include "Engine/Lighting.h"


void Component::BaseMeshRender::Awake()
{
    if(!this->transform)
        this->transform = this->GetOwner()->GetComponent<Component::Transform>();

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
 * @param projection Projection matrix
 * @param view View matrix
 * @return Model matrix
 */
glm::mat4 Component::BaseMeshRender::RenderSetBasics(glm::mat4 &projection, glm::mat4 &view)
{
    this->renderShader->Use();

    this->renderShader->SetMat4("projection", projection);
    this->renderShader->SetMat4("view", view);

    glm::mat4 model = this->transform->GetMatrixTransform();
    this->renderShader->SetMat4("transform", model);

    return model;
}