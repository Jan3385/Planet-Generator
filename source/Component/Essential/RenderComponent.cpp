#include "RenderComponent.h"

#include "Engine/Engine.h"

void Component::RenderComponent::OnMeshUpdated(Mesh *mesh)
{
    // mesh is getting deleted
    if(mesh->IsEmpty()){
        this->SetMeshComponent(nullptr);
        return;
    }

    this->SetMeshData(mesh);
}

void Component::RenderComponent::SetMeshData(Component::Mesh *mesh)
{
    std::vector<float> verticies = mesh->GetVerticies();
    this->verticiesBuffer.SetData(verticies.data(), verticies.size() * sizeof(float), GL_STATIC_DRAW);
    std::vector<unsigned int> indicies = mesh->GetIndicies();
    this->indiciesBuffer.SetData(indicies.data(), indicies.size() * sizeof(unsigned int), GL_STATIC_DRAW);
    
}

void Component::RenderComponent::Awake()
{
    this->verticiesBuffer = GL::Buffer<float, GL::BufferTarget::ArrayBuffer>("RC_VerticiesBuffer");
    this->indiciesBuffer = GL::Buffer<unsigned int, GL::BufferTarget::ElementArrayBuffer>("RC_IndiciesBuffer");
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

void Component::RenderComponent::OnDestroy()
{
    if(this->mesh != nullptr)
        this->mesh->RemoveUpdateCallback(this);
    this->mesh = nullptr;

    GameEngine::renderer->RemoveRenderCallback(this);
}

void Component::RenderComponent::OnEnable()
{
    GameEngine::renderer->AddRenderCallback(this);
}

void Component::RenderComponent::OnDisable()
{
    GameEngine::renderer->RemoveRenderCallback(this);
}

void Component::RenderComponent::Render(glm::mat4 &projection, glm::mat4 &view)
{
    this->renderShader->Use();

    this->renderShader->SetMat4("projection", projection);
    this->renderShader->SetMat4("view", view);

    if(this->transform)
        this->renderShader->SetMat4("transform", this->transform->GetMatrixTransform());

    this->vertexArrayObject.Bind();
    this->indiciesBuffer.Bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->mesh->GetIndicies().size()), GL_UNSIGNED_INT, 0);
}

void Component::RenderComponent::SetMeshComponent(Mesh *newMesh)
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
