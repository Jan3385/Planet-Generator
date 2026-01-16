#include "PhongMeshRenderComponent.h"

#include "Engine/Engine.h"
#include "Engine/Lighting.h"
#include "Debug/Logger.h"

void Component::PhongMeshRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    if(!this->mesh) return;

    glm::mat4 model = this->RenderSetBasics(projection, view);

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    this->renderShader->SetMat3("normalMatrix", normalMatrix);

    this->material->Bind(*this->renderShader);

    auto closestPLights = GameEngine::lighting->GetClosestPointLights(this->transform->GetPos());
    int pointLightCount = 0;

    for (auto* pointLight : closestPLights) {
        if (pointLight != nullptr) {
            pointLight->Bind(*this->renderShader, pointLightCount);
            pointLightCount++;
        }
    }
    this->renderShader->SetInt("numPointLights", pointLightCount);
    
    this->renderShader->SetVec3("viewPos", 
        GameEngine::currentLevel->GetCamera()->GetOwner()->GetComponent<Component::Transform>()->GetPos());

    if(!mesh || mesh->IsEmpty()) {
        Debug::LogWarn("PhongMeshRender: No mesh set or mesh is empty!");
    }

    this->mesh->GLBind();
    this->mesh->GLDraw();
}

Component::PhongMeshRender* Component::PhongMeshRender::SetMeshComponent(Component::GLMesh *newMesh)
{
    this->mesh = newMesh;
    return this;
}

void Component::PhongMeshRender::Awake()
{
    if(!this->transform)
        this->transform = this->GetOwner()->GetComponent<Component::Transform>();

    GameEngine::renderer->AddRenderCallback(this);
}