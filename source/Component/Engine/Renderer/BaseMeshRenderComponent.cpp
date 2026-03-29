#include "BaseMeshRenderComponent.h"

#include "Engine/Engine.h"
#include "Engine/Lighting.h"

void Component::BaseMeshRender::RenderVelocity(GL::Shader &s)
{
    if(!this->transform || !this->mesh) return;

    Renderer::SetReverseFaceCulling(true);

    glm::mat4 model = this->transform->GetMatrixTransform();
    s.SetMat4("transform", model);
    s.SetMat4("prevTransform", this->prevMatrixTransform);
    this->prevMatrixTransform = model;

    this->mesh->Draw();

    Renderer::SetReverseFaceCulling(false);
}

void Component::BaseMeshRender::RenderDepthOnly(GL::Shader &s)
{
    if(!this->transform || !this->mesh) return;

    glm::mat4 model = this->transform->GetMatrixTransform();
    s.SetMat4("transform", model);

    this->mesh->Draw();
}

bool Component::BaseMeshRender::IsInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes)
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

bool Component::BaseMeshRender::GetFrustumData(glm::vec3 &centroid, double &radius, size_t meshIndex)
{
    if(!this->transform || !this->mesh) return false;

    std::vector<GL::Mesh*> meshes = this->mesh->GetMeshes();
    if(meshIndex >= meshes.size()) return false;

    radius = meshes[meshIndex]->GetFrustumRadiusWithCentroid(&centroid, this->transform->GetMatrixTransform());
    return true;
}

glm::vec3 Component::BaseMeshRender::GetPosition() const
{
    if(!this->transform) return glm::vec3(0.0f);

    return this->transform->GetPos();
}

bool Component::BaseMeshRender::IsSphereInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes, glm::vec3 &centroid, double radius)
{
    for (const auto& plane : frustumPlanes) {
        if (glm::dot(glm::vec3(plane), centroid) + plane.w < -radius) {
            return false;
        }
    }

    return true;
}

void Component::BaseMeshRender::Awake()
{
    GameEngine::renderer->AddRenderCallback(this);
}

void Component::BaseMeshRender::OnDestroy()
{
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
 * @return Model matrix
 */
glm::mat4 Component::BaseMeshRender::RenderSetTransform()
{
    this->renderShader->Use();

    glm::mat4 model = this->transform->GetMatrixTransform();
    this->renderShader->SetMat4("transform", model);

    return model;
}