#include "ColorMeshRenderComponent.h"

#include "Object/BaseObject.h"

void Component::ColorMeshRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    if(!this->mesh) return;
    
    this->RenderSetBasics(projection, view);

    this->renderShader->SetVec3("objectColor", this->color);

    this->vertexArrayObject.Bind();
    
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(this->mesh->vertices.size() / 3));
}

void Component::ColorMeshRender::Awake()
{
    this->verticiesBuffer = GL::Buffer<float, GL::BufferTarget::ArrayBuffer>("RC_VerticiesBuffer");
    this->vertexArrayObject = GL::VertexArray("RC_VertexArrayObject");
    
    vertexArrayObject.AddAttribute<glm::vec3>(0, 3, verticiesBuffer, GL_FALSE, 0);
    vertexArrayObject.Unbind();

    BaseMeshRender::Awake();
}

void Component::ColorMeshRender::OnDestroy()
{
    BaseMeshRender::OnDestroy();
}

void Component::ColorMeshRender::OnMeshUpdated(BaseMesh *mesh)
{
    this->SetMeshData(mesh);
}

void Component::ColorMeshRender::SetMeshData(BaseMesh *mesh)
{
    this->verticiesBuffer.SetData(mesh->GetVerticies(), GL_STATIC_DRAW);
}
