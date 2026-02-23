#pragma once

#include "Engine/Renderer.h"

#include "Component/BaseComponent.h"
#include "Component/Essential/TransformComponent.h"

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

    BaseMeshRender* SetRenderShader(GL::Shader* shader) { this->renderShader = shader; return this; }

    virtual void RenderVelocity(GL::Shader &s) override;

    virtual bool IsInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes) override;
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return { }; }

    static bool IsSphereInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes, glm::vec3 &centroid, double radius);

    std::shared_ptr<GL::IMeshRenderable> mesh = nullptr;

    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    glm::mat4 RenderSetTransform();

    GL::Shader *renderShader;

    Transform* transform = nullptr;
    glm::mat4 prevMatrixTransform = glm::mat4(1.0f);

    friend class Object::BaseObject;
};
}