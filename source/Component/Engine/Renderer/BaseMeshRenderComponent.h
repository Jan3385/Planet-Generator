#pragma once

#include "Engine/Renderer.h"

#include "Component/BaseComponent.h"
#include "Component/Engine/TransformComponent.h"
#include "Object/Material/Material.h"

#include "GLWrapper/Buffer.h"
#include "GLWrapper/VertexArray.h"
#include "GLWrapper/Shader.h"
#include "GLWrapper/Mesh.h"

namespace Component {
/**
 * @brief Render component class
 * @details Handles rendering of the object
 * @warning On Awake tries to find a transform and mesh automatically. If transform is found and later deleted, it can cause a crash!
 */
class BaseMeshRender : public BaseComponent, public Renderer::IRendererCallback {
public:
    BaseMeshRender(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~BaseMeshRender() override = default;

    virtual void Render(glm::mat4 &projection, glm::mat4 &view) override = 0;

    virtual void RenderVelocity(GL::Shader &s) override = 0;
    virtual void RenderDepthOnly(GL::Shader &s) override = 0;

    virtual glm::vec3 GetPosition() const override;
protected:
    static bool IsSphereInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes, glm::vec3 &centroid, double radius);

    std::shared_ptr<GL::IMeshRenderable> mesh = nullptr;
    Component::Transform* transform = nullptr;

    void SetRenderCallback(Object::Material::Transparency transparency);
    void UnsetRenderCallback();

private:
    Object::Material::Transparency transparencyCallbackType = Object::Material::Transparency::Special;
    friend class Object::BaseObject;
};
}