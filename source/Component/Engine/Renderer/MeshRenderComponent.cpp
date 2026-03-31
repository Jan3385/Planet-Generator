#include "MeshRenderComponent.h"

#include "Engine/Engine.h"

void Component::MeshRender::Render(glm::mat4 &projection, glm::mat4 &view)
{
    if(!this->mesh)      { Debug::LogSpam("MeshRender: No mesh found for rendering!"); return; }
    if(!this->material)  { Debug::LogSpam("MeshRender: No material found for rendering!"); return; }
    
    Object::Material* mat = this->material;

    if(!this->transform && mat->HasAttribute(Object::Material::RenderAttributes::Transform)) { 
        Debug::LogSpam("MeshRender: No transform found for rendering!"); return; 
    }

    GL::Shader* shader = mat->GetShader();

    shader->Use();
    
    glm::mat4 model(1.0f);
    glm::mat4 prevModel(1.0f);
    if(this->transform){
        model = this->transform->GetMatrixTransform();
        prevModel = this->transform->GetPreviousMatrixTransform();
    }


    if(mat->HasAttribute(Object::Material::RenderAttributes::Transform)){
        shader->SetMat4("transform", model);
    }
    if(mat->HasAttribute(Object::Material::RenderAttributes::PreviousTransform)){
        shader->SetMat4("prevTransform", prevModel);
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
    if(mat->HasAttribute(Object::Material::RenderAttributes::ViewNoTransformMatrix)){
        shader->SetMat4("viewNoTransform", glm::mat4(glm::mat3(view)));
    }
    if(mat->HasAttribute(Object::Material::RenderAttributes::Depth_LEQUAL)){
        glDepthFunc(GL_LEQUAL);
    }

    mat->Bind(*shader);

    this->mesh->Draw();

    if(mat->HasAttribute(Object::Material::RenderAttributes::ReverseFaceCulling)){
        Renderer::SetReverseFaceCulling(false);
    }
    if(mat->HasAttribute(Object::Material::RenderAttributes::Depth_LEQUAL)){
        glDepthFunc(GL_LESS);
    }
}

void Component::MeshRender::RenderVelocity(GL::Shader &s)
{
    if(!this->transform || !this->mesh) return;

    Renderer::SetReverseFaceCulling(true);

    s.SetMat4("transform", this->transform->GetMatrixTransform());
    s.SetMat4("prevTransform", this->transform->GetPreviousMatrixTransform());

    this->mesh->Draw();

    Renderer::SetReverseFaceCulling(false);
}

void Component::MeshRender::RenderDepthOnly(GL::Shader &s)
{
    if(!this->transform || !this->mesh) return;

    s.SetMat4("transform", this->transform->GetMatrixTransform());

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
