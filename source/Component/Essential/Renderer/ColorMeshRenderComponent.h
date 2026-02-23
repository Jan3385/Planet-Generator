#pragma once

#include "Object/Material.h"

#include "Component/Essential/Renderer/BaseMeshRenderComponent.h"

namespace Component {
/**
 * @brief Render component class
 * @details Handles rendering of the object. Renders an object with a flat color
 * @warning On Awake tries to find a transform and mesh automatically. If transform is found and later deleted, it can cause a crash!
 */
class ColorMeshRender : public Component::BaseMeshRender {
public:
    ColorMeshRender(Object::BaseObject* owner);
    ~ColorMeshRender() override = default;

    void Render(glm::mat4 &projection, glm::mat4 &view) override;

    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    void SetMesh(std::shared_ptr<GL::IMeshRenderable> mesh) { this->mesh = mesh; }
    std::shared_ptr<GL::IMeshRenderable> GetMesh() const { return this->mesh; }
protected:
    GL::Buffer<float, GL::BufferTarget::ArrayBuffer> verticiesBuffer;
    GL::VertexArray vertexArrayObject;

    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }
private:

    friend class Object::BaseObject;
};
}