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
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return { }; }


    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    glm::mat4 RenderSetTransform();

    GL::Shader *renderShader;

    Transform* transform = nullptr;

    friend class Object::BaseObject;
};
}