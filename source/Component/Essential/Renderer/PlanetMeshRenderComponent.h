#pragma once

#include "GLWrapper/ColorMesh.h"
#include "Component/Essential/Renderer/BaseMeshRenderComponent.h"

namespace Component {
/**
 * @brief Render component class
 * @details Handles rendering of a planet
 */
class PlanetMeshRender : public Component::BaseMeshRender{
public:
    PlanetMeshRender(Object::BaseObject* owner) : BaseMeshRender(owner) {};
    ~PlanetMeshRender() override = default;

    void Render(glm::mat4 &projection, glm::mat4 &view) override;

    void SetMesh(GL::ColorMesh* mesh) { this->mesh = mesh; }
    GL::ColorMesh* GetMesh() const { return this->mesh; }
protected:
    void Awake() override;
    void OnDestroy() override;

    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }
private:
    GL::ColorMesh *mesh = nullptr;

    friend class Object::BaseObject;
};
}