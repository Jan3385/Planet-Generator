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
    PhongMeshRender(Object::BaseObject* owner) : BaseMeshRender(owner) {};
    ~PhongMeshRender() override = default;

    void Render(glm::mat4 &projection, glm::mat4 &view) override;

    void SetMaterial(Material* newMaterial) { this->material = newMaterial; }
protected:
    Material *material = nullptr;   
    
    void Awake() override;

    void SetMeshData(Mesh* mesh) override;

    GL::Buffer<float, GL::BufferTarget::ArrayBuffer> normalsBuffer;
private:
    friend class Object::BaseObject;
};
}