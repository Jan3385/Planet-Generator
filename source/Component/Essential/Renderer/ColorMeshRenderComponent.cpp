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
    
    this->RenderSetBasics(projection, view);

    this->renderShader->SetVec3("objectColor", this->color);

    this->mesh->Bind();
    this->mesh->Draw();
    
    //glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(this->mesh->vertices.size() / 3));
}

void Component::ColorMeshRender::Awake()
{
    BaseMeshRender::Awake();
}

void Component::ColorMeshRender::OnDestroy()
{
    BaseMeshRender::OnDestroy();
}
