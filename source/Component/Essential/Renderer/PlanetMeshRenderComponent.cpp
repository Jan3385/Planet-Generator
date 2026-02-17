#include "PlanetMeshRenderComponent.h"

#include "Engine/Engine.h"

Component::PlanetMeshRender::PlanetMeshRender(Object::BaseObject *owner)
     : BaseMeshRender(owner)
{
    this->paletteBuffer = GL::Buffer<planetPalette, GL::BufferTarget::UniformBuffer>("PlanetPaletteBuffer");
}

void Component::PlanetMeshRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    glm::mat4 model = this->RenderSetTransform();

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    this->renderShader->SetMat3("normalMatrix", normalMatrix);

    this->paletteBuffer.BindBufferBase(0);

    if(!mesh) {
        Debug::LogWarn("PlanetMeshRender: No mesh set");
    }

    this->mesh->Bind();
    this->mesh->Draw();
}

void Component::PlanetMeshRender::RenderVelocity(GL::Shader &s)
{
    if(!this->transform || !this->mesh) return;

    glm::mat4 model = this->transform->GetMatrixTransform();
    s.SetMat4("transform", model);
    s.SetMat4("prevTransform", this->prevMatrixTransform);
    this->prevMatrixTransform = model;

    this->mesh->Bind();
    this->mesh->Draw();
}

void Component::PlanetMeshRender::SetColorPalette(const planetPalette &palette)
{
    this->paletteBuffer.SetData(palette, GL_STATIC_DRAW);
}

void Component::PlanetMeshRender::Awake()
{
    if(!this->transform)
        this->transform = this->GetOwner()->GetComponent<Component::Transform>();

    GameEngine::renderer->AddRenderCallback(this);
}

void Component::PlanetMeshRender::OnDestroy()
{
    GameEngine::renderer->RemoveRenderCallback(this);
}
