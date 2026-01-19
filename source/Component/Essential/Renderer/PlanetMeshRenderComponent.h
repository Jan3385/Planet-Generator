#pragma once

#include "Component/Essential/Mesh/GLMeshComponent.h"
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
protected:
    void Awake() override;
    void OnDestroy() override;

    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }
private:
    friend class Object::BaseObject;
};
}