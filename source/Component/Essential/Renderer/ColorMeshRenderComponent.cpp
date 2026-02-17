#include "ColorMeshRenderComponent.h"

#include "Object/BaseObject.h"
#include "Engine/Engine.h"

Component::ColorMeshRender::ColorMeshRender(Object::BaseObject *owner)
    : BaseMeshRender(owner)
{
    this->SetRenderShader(&GameEngine::renderer->GetDefaultColorShader());
}

void Component::ColorMeshRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    if(!this->mesh) return;
    
    this->RenderSetTransform();

    this->renderShader->SetVec3("objectColor", this->color);

    this->mesh->Bind();
    this->mesh->Draw();
}

void Component::ColorMeshRender::RenderVelocity(GL::Shader &s)
{
    if(!this->transform || !this->mesh) return;

    glm::mat4 model = this->transform->GetMatrixTransform();
    s.SetMat4("transform", model);
    s.SetMat4("prevTransform", this->prevMatrixTransform);
    this->prevMatrixTransform = model;

    this->mesh->Bind();
    this->mesh->Draw();
}

void Component::ColorMeshRender::Awake()
{
    GameEngine::renderer->AddNoLightRenderCallback(this);

    if(!this->transform)
        this->transform = this->GetOwner()->GetComponent<Component::Transform>();
}

void Component::ColorMeshRender::OnDestroy()
{
    GameEngine::renderer->RemoveNoLightRenderCallback(this);
}

void Component::ColorMeshRender::OnEnable()
{
    GameEngine::renderer->AddNoLightRenderCallback(this);
}

void Component::ColorMeshRender::OnDisable()
{
    GameEngine::renderer->RemoveNoLightRenderCallback(this);
}
