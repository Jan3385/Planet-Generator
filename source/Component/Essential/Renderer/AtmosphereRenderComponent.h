#pragma once

#include "GLWrapper/Mesh.h"
#include "Component/Essential/Renderer/BaseMeshRenderComponent.h"

namespace Component {
/**
 * @brief Render component class
 * @details Handles rendering of a planet's atmosphere
 */
class AtmosphereRender : public Component::BaseMeshRender{
public:
    struct alignas(16) atmospherePalette{
        glm::vec4 horizonColor;
        glm::vec4 zenithColor;
    };

    AtmosphereRender(Object::BaseObject* owner);
    ~AtmosphereRender() override = default;

    void Render(glm::mat4 &projection, glm::mat4 &view) override;
    void RenderVelocity(GL::Shader &s) override;

    bool IsInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes) override;

    void SetColorPalette(const atmospherePalette& palette);
protected:
    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }
private:
    std::shared_ptr<GL::Mesh> mesh = nullptr;
    GL::Buffer<atmospherePalette, GL::BufferTarget::UniformBuffer> paletteBuffer;

    friend class Object::BaseObject;
};
}