#pragma once

#include "Component/BaseComponent.h"
#include "Component/Essential/TransformComponent.h"
#include "Engine/Lighting.h"
#include "GLWrapper/FrameBuffer.h"

namespace Component {
/**
 * 
 */
class PointLightSource : public BaseComponent, public IUpdatable {
public:
    struct PointLightData{
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float intensity = 1.0f;

        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    };

    PointLightSource* SetLightData(const PointLightData& newData);
    PointLightSource* SetLightData(glm::vec3 color, float linear, float quadratic, float radius);

    void RenderShadowMap(GL::Shader& shadowShader);

    PointLightSource(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~PointLightSource() override = default;
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }
private:
    Lighting::PointLightSourceData data;
    GL::FrameBuffer shadowFBO;

    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    void Update() override;
    void FixedUpdate() override {};

    Transform* transform = nullptr;

    friend class Object::BaseObject;
};
}