#pragma once

#include "GLWrapper/Cubemap.h"
#include "Component/Essential/Renderer/BaseMeshRenderComponent.h"

namespace Component {

class SkyboxRender : public Component::BaseMeshRender{
public:
    SkyboxRender(Object::BaseObject* owner);
    ~SkyboxRender() override = default;

    void LoadCubemap(const std::string filePaths[6], bool blurred = false);

    void Render(glm::mat4 &projection, glm::mat4 &view) override;
protected:
    void Awake() override     {};
    void OnDestroy() override {};

    void OnEnable() override  {};
    void OnDisable() override {};

    std::vector<std::type_index> GetDependencies() const override 
        { return {/*typeid(Component::Transform)*/}; }
private:
    GL::Cubemap cubemap;
    std::shared_ptr<GL::Mesh> cubeMesh = nullptr;
    friend class Object::BaseObject;
};
}