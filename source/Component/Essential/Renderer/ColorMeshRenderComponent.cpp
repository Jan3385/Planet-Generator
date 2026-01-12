#include "ColorMeshRenderComponent.h"

void Component::ColorMeshRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    this->RenderSetBasics(projection, view);

    this->renderShader->SetVec3("objectColor", this->color);

    this->vertexArrayObject.Bind();
    
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(this->mesh->GetVerticies().size() / 3));
}