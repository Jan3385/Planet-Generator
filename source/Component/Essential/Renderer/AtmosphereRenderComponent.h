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

        BaseMeshRender* SetRenderShaderInside(GL::Shader* shader) { this->renderShaderInside = shader; return this; }

    void Render(glm::mat4 &projection, glm::mat4 &view) override;

    void SetColorPalette(const atmospherePalette& palette);
protected:
    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    void RenderInside(glm::mat4 &projection, glm::mat4 &view, glm::mat4 &model, glm::mat3 &normalMatrix);
    void RenderOutside(glm::mat4 &projection, glm::mat4 &view, glm::mat4 &model, glm::mat3 &normalMatrix);

    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }
private:
    std::shared_ptr<GL::Mesh> mesh = nullptr;
    GL::Buffer<atmospherePalette, GL::BufferTarget::UniformBuffer> paletteBuffer;

    GL::Shader *renderShaderInside = nullptr;

    friend class Object::BaseObject;
};
}