#include "PhongMeshRenderComponent.h"

#include "Engine/Engine.h"
#include "Engine/Lighting.h"
#include "Debug/Logger.h"

Component::PhongMeshRender::PhongMeshRender(Object::BaseObject *owner)
    : BaseMeshRender(owner)
{
    this->SetRenderShader(&GameEngine::renderer->GetDefaultLightShader());
}

void Component::PhongMeshRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    glm::mat4 model = this->RenderSetTransform();

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    this->renderShader->SetMat3("normalMatrix", normalMatrix);

    this->material->Bind(*this->renderShader);

    if(!mesh) {
        Debug::LogWarn("PhongMeshRender: No mesh set");
    }

    this->mesh->Bind();
    this->mesh->Draw();
}

void Component::PhongMeshRender::Awake()
{
    if(!this->transform)
        this->transform = this->GetOwner()->GetComponent<Component::Transform>();

    GameEngine::renderer->AddRenderCallback(this);
}