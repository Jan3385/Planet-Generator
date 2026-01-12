#pragma once

#include "Engine/Renderer.h"

#include "Component/BaseComponent.h"
#include "Component/Essential/MeshComponent.h"
#include "Component/Essential/TransformComponent.h"

#include "GLWrapper/Buffer.h"
#include "GLWrapper/VertexArray.h"
#include "GLWrapper/Shader.h"

namespace Component {
/**
 * @brief Render component class
 * @details Handles rendering of the object
 * @warning On Awake tries to find a transform and mesh automatically. If transform is found and later deleted, it can cause a crash!
 */
class BaseMeshRender : public BaseComponent, public Mesh::IMeshUpdateCallback, public Renderer::IRendererCallback {
public:
    BaseMeshRender(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~BaseMeshRender() override = default;

    void Render(glm::mat4 &projection, glm::mat4 &view) override;

    Mesh* GetMesh() const { return mesh; }
    void SetMeshComponent(Mesh* newMesh);
    void SetRenderShader(GL::Shader* shader) { this->renderShader = shader; }
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }

    void OnMeshUpdated(Mesh* mesh) override;

    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    glm::mat4 RenderSetBasics(glm::mat4 &projection, glm::mat4 &view);

    Mesh* mesh = nullptr;

    GL::Buffer<float, GL::BufferTarget::ArrayBuffer> verticiesBuffer;
    GL::VertexArray vertexArrayObject;
    GL::Shader *renderShader;

    Transform* transform = nullptr;

    virtual void SetMeshData(Mesh* mesh);

    friend class Object::BaseObject;
};
}