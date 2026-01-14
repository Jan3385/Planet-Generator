#include "ColorMeshRenderComponent.h"

#include "Object/BaseObject.h"

void Component::ColorMeshRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    if(!this->mesh) return;
    
    this->RenderSetBasics(projection, view);

    this->renderShader->SetVec3("objectColor", this->color);

    this->vertexArrayObject.Bind();
    
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(this->mesh->GetVerticies().size() / 3));
}

void Component::ColorMeshRender::SetMeshComponent(BaseMesh *newMesh)
{
    if(this->mesh != nullptr)
        this->mesh->RemoveUpdateCallback(this);

    if(newMesh != nullptr){
        newMesh->AddUpdateCallback(this);
        
        if(this->IsAwake())
            this->SetMeshData(newMesh);
    }

    this->mesh = newMesh;
}

void Component::ColorMeshRender::Awake()
{
    this->verticiesBuffer = GL::Buffer<float, GL::BufferTarget::ArrayBuffer>("RC_VerticiesBuffer");
    this->vertexArrayObject = GL::VertexArray("RC_VertexArrayObject");
    
    vertexArrayObject.AddAttribute<glm::vec3>(0, 3, verticiesBuffer, GL_FALSE, 0);
    vertexArrayObject.Unbind();

    if(!this->mesh)
        this->SetMeshComponent(this->GetOwner()->GetComponent<Component::SimpleMesh>());

    if(this->mesh)
        this->SetMeshData(this->mesh);

    BaseMeshRender::Awake();
}

void Component::ColorMeshRender::OnDestroy()
{
    if(this->mesh != nullptr)
        this->mesh->RemoveUpdateCallback(this);
    this->mesh = nullptr;

    BaseMeshRender::OnDestroy();
}

void Component::ColorMeshRender::OnMeshUpdated(BaseMesh *mesh)
{
     // mesh is getting deleted
    if(mesh->IsEmpty()){
        this->SetMeshComponent(nullptr);
        return;
    }

    this->SetMeshData(mesh);
}

void Component::ColorMeshRender::SetMeshData(BaseMesh *mesh)
{
    this->verticiesBuffer.SetData(mesh->GetVerticies(), GL_STATIC_DRAW);
}
