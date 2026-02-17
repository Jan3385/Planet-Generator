#pragma once

#include "Object/Material.h"

#include "Component/Essential/Renderer/BaseMeshRenderComponent.h"

namespace Component {
/**
 * @brief Render component class
 * @details Handles rendering of the object. Uses Phong shading and lighting model
 * @warning On Awake tries to find a transform and mesh automatically. If transform is found and later deleted, it can cause a crash!
 */
class PhongMeshRender : public Component::BaseMeshRender {
public:
    PhongMeshRender(Object::BaseObject* owner);
    ~PhongMeshRender() override = default;

    void Render(glm::mat4 &projection, glm::mat4 &view) override;
    void RenderVelocity(GL::Shader &s) override;

    bool IsInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes) override{
        glm::vec3 centroid;
        double radius = this->mesh->GetFrustumRadiusWithCentroid(&centroid, this->transform->GetPos(), this->transform->GetScale());
        return Component::BaseMeshRender::IsInsideFrustum(frustumPlanes, centroid, radius);
    }

    PhongMeshRender* SetMaterial(Material* newMaterial) { this->material = newMaterial; return this; }

    void SetMesh(std::shared_ptr<GL::Mesh> mesh) { this->mesh = mesh; }
    std::shared_ptr<GL::Mesh> GetMesh() const { return this->mesh; }
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }

    Material *material = nullptr;   
    
    void Awake() override;
private:
    std::shared_ptr<GL::Mesh> mesh = nullptr;

    friend class Object::BaseObject;
};
}