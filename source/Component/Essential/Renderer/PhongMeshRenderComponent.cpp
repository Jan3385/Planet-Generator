#include "PhongMeshRenderComponent.h"

#include "Engine/Engine.h"
#include "Engine/Lighting.h"

void Component::PhongMeshRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    if(!this->mesh) return;

    glm::mat4 model = this->RenderSetBasics(projection, view);

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    this->renderShader->SetMat3("normalMatrix", normalMatrix);

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
            this->renderShader->SetFloat(baseName + ".constant", light->constant);
            this->renderShader->SetFloat(baseName + ".linear", light->linear);
            this->renderShader->SetFloat(baseName + ".quadratic", light->quadratic);
            lightCount++;
        }
    }
    this->renderShader->SetInt("numPointLights", lightCount);
    
    this->renderShader->SetVec3("viewPos", 
        GameEngine::currentLevel->GetCamera()->GetOwner()->GetComponent<Component::Transform>()->GetPos());

    this->vertexArrayObject.Bind();

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(this->mesh->GetVerticies().size() / 3));
}

void Component::PhongMeshRender::Awake()
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

void Component::PhongMeshRender::SetMeshData(Mesh *mesh)
{
    BaseMeshRender::SetMeshData(mesh);

    this->normalsBuffer.SetData(mesh->GetNormals(), GL_STATIC_DRAW);
}