#pragma once

#include "Engine/Renderer.h"

#include "Object/Material/Material.h"
#include "Component/BaseComponent.h"
#include "Component/Engine/TransformComponent.h"

namespace Component {
class MeshRender : public BaseComponent, public Renderer::IRendererCallback  {
public:
    MeshRender(Object::BaseObject* owner) : BaseComponent(owner) {};

    virtual void Render(glm::mat4 &projection, glm::mat4 &view) override;

    virtual void RenderVelocity(GL::Shader &s) override;
    virtual void RenderDepthOnly(GL::Shader &s) override;

    virtual bool IsInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes) override;
    virtual bool GetFrustumData(glm::vec3 &centroid, double &radius, size_t meshIndex) override;

    virtual glm::vec3 GetPosition() const override;

    MeshRender* SetMesh(std::shared_ptr<GL::IMeshRenderable> mesh)  { this->mesh = mesh; return this; }
    MeshRender* SetTransform(Component::Transform* transform)       { this->transform = transform; return this; }
    MeshRender* SetMaterial(Object::Material* material);
protected:
    static bool IsSphereInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes, glm::vec3 &centroid, double radius);
private:
    std::vector<std::type_index> GetDependencies() const override 
        { return { }; }

    std::shared_ptr<GL::IMeshRenderable> mesh = nullptr;
    Object::Material* material = nullptr;

    Component::Transform* transform = nullptr;

    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override  { this->SetRenderCallback();   };
    void OnDisable() override { this->UnsetRenderCallback(); };

    void SetRenderCallback();
    void UnsetRenderCallback();

    friend class Object::BaseObject;
};
}