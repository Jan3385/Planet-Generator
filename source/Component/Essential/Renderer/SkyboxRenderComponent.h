#pragma once

#include "GLWrapper/Cubemap.h"
#include "Component/Essential/Renderer/BaseMeshRenderComponent.h"

namespace Component {

class SkyboxRender : public Component::BaseMeshRender{
public:
    SkyboxRender(Object::BaseObject* owner);
    ~SkyboxRender() override = default;

    void LoadCubemap(const std::string filePaths[6], bool blurred = false, bool sRGB = false);

    void Render(glm::mat4 &projection, glm::mat4 &view) override;

    bool IsInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes) override { return true; }
protected:
    void Awake() override     {};
    void OnDestroy() override {};

    void OnEnable() override  {};
    void OnDisable() override {};

    std::vector<std::type_index> GetDependencies() const override 
        { return { }; }
private:
    GL::Cubemap cubemap;
    friend class Object::BaseObject;
};
}