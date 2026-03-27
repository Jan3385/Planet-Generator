#include "MeshRenderComponent.h"

#include "Engine/Engine.h"

void Component::MeshRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    if(!this->transform) { Debug::LogSpam("MeshRender: No transform found for rendering!"); return; }
    if(!this->mesh)      { Debug::LogSpam("MeshRender: No mesh found for rendering!"); return; }
    if(!this->material)  { Debug::LogSpam("MeshRender: No material found for rendering!"); return; }

    Object::Material* mat = this->material;
    GL::Shader* shader = mat->GetShader();

    shader->Use();
    
    glm::mat4 model = this->transform->GetMatrixTransform();
    if(mat->HasAttribute(Object::Material::RenderAttributes::Transform)){
        shader->SetMat4("transform", model);
    }
    if(mat->HasAttribute(Object::Material::RenderAttributes::PreviousTransform)){
        shader->SetMat4("prevTransform", this->prevMatrixTransform);
        this->prevMatrixTransform = this->transform->GetMatrixTransform();
    }
    if(mat->HasAttribute(Object::Material::RenderAttributes::NormalMatrix)){
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        shader->SetMat3("normalMatrix", normalMatrix);
    }
    if(mat->HasAttribute(Object::Material::RenderAttributes::ReverseFaceCulling)){
        Renderer::SetReverseFaceCulling(true);
    }
    if(mat->HasAttribute(Object::Material::RenderAttributes::OriginPos)){
        shader->SetVec3("originPos", this->transform->GetPos());
    }
    if(mat->HasAttribute(Object::Material::RenderAttributes::ViewMatrix)){
        shader->SetMat4("view", view);
    }

    mat->Bind(*shader);

    this->mesh->Draw();

    if(mat->HasAttribute(Object::Material::RenderAttributes::ReverseFaceCulling)){
        Renderer::SetReverseFaceCulling(false);
    }
}

void Component::MeshRender::RenderVelocity(GL::Shader &s)
{
    if(!this->transform || !this->mesh) return;

    Renderer::SetReverseFaceCulling(true);

    //FIXME: GetMatrixTransform can destroy the prevMatrix sync in other render components
    glm::mat4 model = this->transform->GetMatrixTransform();
    s.SetMat4("transform", model);
    s.SetMat4("prevTransform", this->prevMatrixTransform);
    this->prevMatrixTransform = model;

    this->mesh->Draw();

    Renderer::SetReverseFaceCulling(false);
}

void Component::MeshRender::RenderDepthOnly(GL::Shader &s)
{
    if(!this->transform || !this->mesh) return;

    glm::mat4 model = this->transform->GetMatrixTransform();
    s.SetMat4("transform", model);

    this->mesh->Draw();
}

bool Component::MeshRender::IsInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes)
{
    if(!this->transform || !this->mesh) return false;

    // if any part of renderable mesh is inside frustum render it
    glm::vec3 centroid;
    double radius;
    for(size_t i = 0; this->GetFrustumData(centroid, radius, i); i++){
        if(IsSphereInsideFrustum(frustumPlanes, centroid, radius)) return true;
    }

    return false;
}

/// @brief Returns the centroid and radius for the mesh at a given index
/// @param centroid [OUT] centroid of the mesh
/// @param radius [OUT] radius required to fit the mesh in a sphere
/// @param meshIndex [IN] index of the mesh 
/// @return true if the mesh was correctly retrieved, otherwise false
bool Component::MeshRender::GetFrustumData(glm::vec3 &centroid, double &radius, size_t meshIndex)
{
    if(!this->transform || !this->mesh) return false;

    std::vector<GL::Mesh*> meshes = this->mesh->GetMeshes();
    if(meshIndex >= meshes.size()) return false;

    radius = meshes[meshIndex]->GetFrustumRadiusWithCentroid(&centroid, this->transform->GetMatrixTransform());
    return true;
}

Component::MeshRender *Component::MeshRender::SetMaterial(Object::Material *material)
{
    if(this->material && this->material->transparency == material->transparency){
        this->material = material;
        return this;
    }

    this->UnsetRenderCallback();
    this->material = material;
    this->SetRenderCallback();
    return this;
}

bool Component::MeshRender::IsSphereInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes, glm::vec3 &centroid, double radius)
{
    for (const auto& plane : frustumPlanes) {
        if (glm::dot(glm::vec3(plane), centroid) + plane.w < -radius) {
            return false;
        }
    }

    return true;
}

void Component::MeshRender::Awake()
{
    this->transform = this->GetOwner()->GetComponent<Component::Transform>();

    this->SetRenderCallback();
}

void Component::MeshRender::OnDestroy()
{
    this->UnsetRenderCallback();
}

void Component::MeshRender::SetRenderCallback()
{
    if(!this->material) return;

    switch (this->material->transparency)
    {
    case Object::Material::Transparency::Special:
        break;
    case Object::Material::Transparency::Opaque:
        GameEngine::renderer->AddRenderCallback(this);
        break;
    case Object::Material::Transparency::Transparent:
        GameEngine::renderer->AddTransparentRenderCallback(this);
        break;
    case Object::Material::Transparency::OpaqueNoLight:
        GameEngine::renderer->AddNoLightRenderCallback(this);
        break;
    default:
        Debug::LogWarn("MeshRender: Unknown transparency type in SetRenderCallback");
        break;
    }
}

void Component::MeshRender::UnsetRenderCallback()
{
    if(!this->material) return;

    switch (this->material->transparency)
    {
    case Object::Material::Transparency::Special:
        break;
    case Object::Material::Transparency::Opaque:
        GameEngine::renderer->RemoveRenderCallback(this);
        break;
    case Object::Material::Transparency::Transparent:
        GameEngine::renderer->RemoveTransparentRenderCallback(this);
        break;
    case Object::Material::Transparency::OpaqueNoLight:
        GameEngine::renderer->RemoveNoLightRenderCallback(this);
        break;
    default:
        Debug::LogWarn("MeshRender: Unknown transparency type in UnsetRenderCallback");
        break;
    }
}
