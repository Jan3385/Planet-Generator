#pragma once

#include "Engine/Renderer.h"

#include "Component/BaseComponent.h"
#include "Component/Essential/MeshComponent.h"

#include "GLWrapper/Buffer.h"
#include "GLWrapper/VertexArray.h"
#include "GLWrapper/Shader.h"

namespace Component {
class RenderComponent : public BaseComponent, public Mesh::IMeshUpdateCallback, public Renderer::IRendererCallback {
public:
    RenderComponent(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~RenderComponent() override = default;

    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    void Render() override;

    Mesh* GetMesh() const { return mesh; }
    void SetMeshComponent(Mesh* newMesh);
    void SetRenderShader(GL::Shader* shader) { this->renderShader = shader; }
protected:
    void OnMeshUpdated(Mesh* mesh) override;
private:
    void SetMeshData(Mesh* mesh);
    Mesh* mesh = nullptr;

    GL::Buffer<float, GL::BufferTarget::ArrayBuffer> verticiesBuffer;
    GL::VertexArray vertexArrayObject;
    GL::Buffer<unsigned int, GL::BufferTarget::ElementArrayBuffer> indiciesBuffer;
    GL::Shader *renderShader;
};
}