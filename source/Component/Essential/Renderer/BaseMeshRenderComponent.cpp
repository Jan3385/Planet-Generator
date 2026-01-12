#include "BaseMeshRenderComponent.h"

#include "Engine/Engine.h"
#include "Engine/Lighting.h"

void Component::BaseMeshRender::OnMeshUpdated(Mesh *mesh)
{
    // mesh is getting deleted
    if(mesh->IsEmpty()){
        this->SetMeshComponent(nullptr);
        return;
    }

    this->SetMeshData(mesh);
}

void Component::BaseMeshRender::SetMeshData(Component::Mesh *mesh)
{
    this->verticiesBuffer.SetData(mesh->GetVerticies(), GL_STATIC_DRAW);
}

void Component::BaseMeshRender::Awake()
{
    this->verticiesBuffer = GL::Buffer<float, GL::BufferTarget::ArrayBuffer>("RC_VerticiesBuffer");
    this->vertexArrayObject = GL::VertexArray("RC_VertexArrayObject");
    
    vertexArrayObject.AddAttribute<glm::vec3>(0, 3, verticiesBuffer, GL_FALSE, 0);
    vertexArrayObject.Unbind();

    if(!this->transform)
        this->transform = this->GetOwner()->GetComponent<Component::Transform>();

    if(!this->mesh)
        this->SetMeshComponent(this->GetOwner()->GetComponent<Component::Mesh>());

    if(this->mesh)
        this->SetMeshData(this->mesh);

    GameEngine::renderer->AddRenderCallback(this);
}

void Component::BaseMeshRender::OnDestroy()
{
    if(this->mesh != nullptr)
        this->mesh->RemoveUpdateCallback(this);
    this->mesh = nullptr;

    GameEngine::renderer->RemoveRenderCallback(this);
}

void Component::BaseMeshRender::OnEnable()
{
    GameEngine::renderer->AddRenderCallback(this);
}

void Component::BaseMeshRender::OnDisable()
{
    GameEngine::renderer->RemoveRenderCallback(this);
}

/**
 * @brief Sets up basic rendering uniforms
 * @param projection Projection matrix
 * @param view View matrix
 * @return Model matrix
 */
glm::mat4 Component::BaseMeshRender::RenderSetBasics(glm::mat4 &projection, glm::mat4 &view)
{
    this->renderShader->Use();

    this->renderShader->SetMat4("projection", projection);
    this->renderShader->SetMat4("view", view);

    glm::mat4 model = this->transform->GetMatrixTransform();
    this->renderShader->SetMat4("transform", model);

    return model;
}

void Component::BaseMeshRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    this->RenderSetBasics(projection, view);

    this->vertexArrayObject.Bind();
    
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(this->mesh->GetVerticies().size() / 3));
}

void Component::BaseMeshRender::SetMeshComponent(Mesh *newMesh)
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
