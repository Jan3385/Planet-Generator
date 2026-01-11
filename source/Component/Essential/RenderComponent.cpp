#include "RenderComponent.h"

#include "Engine/Engine.h"
#include "Engine/Lighting.h"

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
    std::vector<float> normals = mesh->GetNormals();
    this->normalsBuffer.SetData(normals.data(), normals.size() * sizeof(float), GL_STATIC_DRAW);
}

void Component::RenderComponent::Awake()
{
    this->verticiesBuffer = GL::Buffer<float, GL::BufferTarget::ArrayBuffer>("RC_VerticiesBuffer");
    this->normalsBuffer = GL::Buffer<float, GL::BufferTarget::ArrayBuffer>("RC_NormalsBuffer");
    this->vertexArrayObject = GL::VertexArray("RC_VertexArrayObject");

    vertexArrayObject.AddAttribute<glm::vec3>(0, 3, verticiesBuffer, GL_FALSE, 0);
    vertexArrayObject.AddAttribute<glm::vec3>(1, 3, normalsBuffer, GL_FALSE, 0);
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
    if(!this->material && this->passLightDataToShader){ //TODO: make new component for light render
        Debug::LogError("RenderComponent rendered with no material assigned!");
        return;
    }
    if(!this->mesh){
        Debug::LogError("RenderComponent rendered with no mesh assigned!");
        return;
    }
    if(!this->transform){
        Debug::LogError("RenderComponent rendered with no transform assigned!");
        return;
    }

    this->renderShader->Use();

    this->renderShader->SetMat4("projection", projection);
    this->renderShader->SetMat4("view", view);

    glm::mat4 model = this->transform->GetMatrixTransform();

    this->renderShader->SetMat4("transform", model);

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    this->renderShader->SetMat3("normalMatrix", normalMatrix);

    if(this->passLightDataToShader) {
        auto closestLights = GameEngine::lighting->GetClosestPointLights(this->transform->GetPos());
        int lightCount = 0;

        this->renderShader->SetVec3("material.ambient", this->material->ambient);
        this->renderShader->SetVec3("material.diffuse", this->material->diffuse);
        this->renderShader->SetVec3("material.specular", this->material->specular);
        this->renderShader->SetFloat("material.shininess", this->material->shininess);

        for (auto* light : closestLights) {
            if (light != nullptr) {
                std::string baseName = "pointLights[" + std::to_string(lightCount) + "]";
                this->renderShader->SetVec3(baseName + ".position", light->position);
                this->renderShader->SetVec3(baseName + ".diffuse", light->diffuse);
                this->renderShader->SetVec3(baseName + ".specular", light->specular);
                lightCount++;
            }
        }
        this->renderShader->SetInt("numPointLights", lightCount);
        
        this->renderShader->SetVec3("viewPos", 
            GameEngine::currentLevel->GetCamera()->GetOwner()->GetComponent<Component::Transform>()->GetPos());
    }

    this->vertexArrayObject.Bind();
    //glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->mesh->GetIndicies().size()), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(this->mesh->GetVerticies().size() / 3));
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
