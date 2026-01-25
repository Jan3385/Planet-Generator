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
    // custom basic matrix setup
    this->renderShader->Use();
    this->renderShader->SetMat4("projection", projection);
    this->renderShader->SetMat4("view", view);

    glm::mat4 model = this->transform->GetMatrixTransform();
    model = glm::scale(model, glm::vec3(1.0f) + PlanetGen::PLANET_SCALE * 0.11f);
    this->renderShader->SetMat4("transform", model);
    // ---

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    this->renderShader->SetMat3("normalMatrix", normalMatrix);

    this->renderShader->SetVec3("viewPos", 
        GameEngine::currentLevel->GetCamera()->GetOwner()->GetComponent<Component::Transform>()->GetPos());

    this->paletteBuffer.BindBufferBase(0);

    if(!mesh) {
        Debug::LogWarn("AtmosphereRender: No mesh set");
    }

    this->mesh->Bind();
    this->mesh->Draw();
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
