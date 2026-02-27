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
    struct alignas(16) MaterialSTD140{
        glm::vec4 color;
        glm::vec4 MetalRought; // .r = metallic, .g = roughness
        MaterialSTD140(glm::vec3 color, glm::vec2 MetalRought)
            : color(color, 1), MetalRought(MetalRought.x, MetalRought.y, 0, 0) {};
    };
    struct alignas(16) planetPalette{
        MaterialSTD140 deepOcean;
        MaterialSTD140 shallowOcean;
        MaterialSTD140 sand;
        MaterialSTD140 grass;
        MaterialSTD140 rock;
        MaterialSTD140 snow;
    };

    PlanetMeshRender(Object::BaseObject* owner);
    ~PlanetMeshRender() override = default;

    void Render(glm::mat4 &projection, glm::mat4 &view) override;

    void SetMesh(std::shared_ptr<GL::IMeshRenderable> mesh) { this->mesh = mesh; }
    void SetColorPalette(const planetPalette& palette);
    std::shared_ptr<GL::IMeshRenderable> GetMesh() const { return this->mesh; }
protected:
    void Awake() override;
    void OnDestroy() override;

    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }
private:
    GL::Buffer<planetPalette, GL::BufferTarget::UniformBuffer> paletteBuffer;

    friend class Object::BaseObject;
};
}