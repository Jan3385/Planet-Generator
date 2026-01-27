#include "AtmosphereRenderComponent.h"

#include "Engine/Engine.h"
#include "Generator/MeshGenerator.h"
#include "Component/Planet/PlanetGenComponent.h"

Component::AtmosphereRender::AtmosphereRender(Object::BaseObject *owner)
        : BaseMeshRender(owner)
{
    this->paletteBuffer = GL::Buffer<atmospherePalette, GL::BufferTarget::UniformBuffer>("AtmospherePaletteBuffer");
}

void Component::AtmosphereRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    glm::mat4 model = this->transform->GetMatrixTransform();
    model = glm::scale(model, glm::vec3(1.0f) + PlanetGen::PLANET_SCALE * 0.11f);

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    this->paletteBuffer.BindBufferBase(0);

    bool isInsideSphere = glm::length(
        GameEngine::currentLevel->GetCamera()->GetPosition()
        - this->transform->GetPos()
    ) < PlanetGen::PLANET_SCALE + this->transform->GetScale().x + 0.1f;

    if(isInsideSphere)
        this->RenderInside(projection, view, model, normalMatrix);
    else
        this->RenderOutside(projection, view, model, normalMatrix);
}

void Component::AtmosphereRender::SetColorPalette(const atmospherePalette &palette)
{
    this->paletteBuffer.SetData(palette, GL_STATIC_DRAW);
}

void Component::AtmosphereRender::Awake()
{
    if(!this->transform)
        this->transform = this->GetOwner()->GetComponent<Component::Transform>();

    GameEngine::renderer->AddTransparentRenderCallback(this);

    this->mesh = MeshGenerator::GenerateSpherifiedCubeMesh(20);
}

void Component::AtmosphereRender::OnDestroy()
{
    GameEngine::renderer->RemoveTransparentRenderCallback(this);
}

void Component::AtmosphereRender::OnEnable()
{
    GameEngine::renderer->AddTransparentRenderCallback(this);
}

void Component::AtmosphereRender::OnDisable()
{
    GameEngine::renderer->RemoveTransparentRenderCallback(this);
}

void Component::AtmosphereRender::RenderInside(glm::mat4 &projection, glm::mat4 &view, glm::mat4 &model, glm::mat3 &normalMatrix)
{
    Renderer::SetFaceCulling(true);
    this->renderShaderInside->Use();
    this->renderShaderInside->SetMat4("projection", projection);
    this->renderShaderInside->SetMat4("view", view);
    this->renderShaderInside->SetMat4("transform", model);
    this->renderShaderInside->SetMat3("normalMatrix", normalMatrix);

    glm::vec3 viewPos = 
        GameEngine::currentLevel->GetCamera()->GetPosition();
    this->renderShaderInside->SetVec3("viewPos", viewPos);

    if(!mesh) {
        Debug::LogWarn("AtmosphereRender: No mesh set");
    }

    this->mesh->Bind();
    this->mesh->Draw();
    Renderer::SetFaceCulling(false);
}

void Component::AtmosphereRender::RenderOutside(glm::mat4 &projection, glm::mat4 &view, glm::mat4 &model, glm::mat3 &normalMatrix)
{
    Renderer::SetFaceCulling(true);
    this->renderShader->Use();
    this->renderShader->SetMat4("projection", projection);
    this->renderShader->SetMat4("view", view);
    this->renderShader->SetMat4("transform", model);
    this->renderShader->SetMat3("normalMatrix", normalMatrix);

    glm::vec3 viewPos = 
        GameEngine::currentLevel->GetCamera()->GetPosition();
    this->renderShader->SetVec3("viewPos", viewPos);

    if(!mesh) {
        Debug::LogWarn("AtmosphereRender: No mesh set");
    }

    this->mesh->Bind();
    this->mesh->Draw();
    Renderer::SetFaceCulling(false);
}
