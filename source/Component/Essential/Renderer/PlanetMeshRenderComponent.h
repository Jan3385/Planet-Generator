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
    struct planetPalette{
        glm::vec4 deepOcean;
        glm::vec4 shallowOcean;
        glm::vec4 sand;
        glm::vec4 grass;
        glm::vec4 rock;
        glm::vec4 snow;
    };

    PlanetMeshRender(Object::BaseObject* owner);
    ~PlanetMeshRender() override = default;

    void Render(glm::mat4 &projection, glm::mat4 &view) override;

    void SetMesh(GL::Mesh* mesh) { this->mesh = mesh; }
    void SetColorPalette(const planetPalette& palette);
    GL::Mesh* GetMesh() const { return this->mesh; }
protected:
    void Awake() override;
    void OnDestroy() override;

    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }
private:
    GL::Mesh *mesh = nullptr;
    GL::Buffer<planetPalette, GL::BufferTarget::UniformBuffer> paletteBuffer;

    friend class Object::BaseObject;
};
}