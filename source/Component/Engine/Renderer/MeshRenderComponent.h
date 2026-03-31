#pragma once

#include "Engine/Renderer.h"

#include "Component/Engine/Renderer/BaseMeshRenderComponent.h"
#include "Object/Material/Material.h"
#include "Component/BaseComponent.h"
#include "Component/Engine/TransformComponent.h"

namespace Component {
class MeshRender : public BaseMeshRender {
public:
    MeshRender(Object::BaseObject* owner) : BaseMeshRender(owner) {};

    virtual void Render(glm::mat4 &projection, glm::mat4 &view) override;

    virtual void RenderVelocity(GL::Shader &s) override;
    virtual void RenderDepthOnly(GL::Shader &s) override;

    virtual bool IsInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes) override;
    virtual bool GetFrustumData(glm::vec3 &centroid, double &radius, size_t meshIndex) override;

    MeshRender* SetMesh(std::shared_ptr<GL::IMeshRenderable> mesh)  { this->mesh = mesh; return this; }
    MeshRender* SetTransform(Component::Transform* transform)       { this->transform = transform; return this; }
    MeshRender* SetMaterial(Object::Material* material);
private:
    std::vector<std::type_index> GetDependencies() const override 
        { return { }; }

    Object::Material* material = nullptr;

    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override  { this->SetRenderCallback();   };
    void OnDisable() override { this->UnsetRenderCallback(); };

    void SetRenderCallback();
    void UnsetRenderCallback();

    friend class Object::BaseObject;
};
}