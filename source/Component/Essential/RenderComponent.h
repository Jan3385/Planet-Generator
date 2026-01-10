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
class RenderComponent : public BaseComponent, public Mesh::IMeshUpdateCallback, public Renderer::IRendererCallback {
public:
    RenderComponent(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~RenderComponent() override = default;

    void Render(glm::mat4 &projection, glm::mat4 &view) override;

    Mesh* GetMesh() const { return mesh; }
    void SetMeshComponent(Mesh* newMesh);
    void SetRenderShader(GL::Shader* shader) { this->renderShader = shader; }
protected:
    void OnMeshUpdated(Mesh* mesh) override;
private:
    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    void SetMeshData(Mesh* mesh);
    Transform* transform = nullptr;
    Mesh* mesh = nullptr;

    GL::Buffer<float, GL::BufferTarget::ArrayBuffer> verticiesBuffer;
    GL::VertexArray vertexArrayObject;
    GL::Buffer<unsigned int, GL::BufferTarget::ElementArrayBuffer> indiciesBuffer;
    GL::Shader *renderShader;

    friend class Object::BaseObject;
};
}