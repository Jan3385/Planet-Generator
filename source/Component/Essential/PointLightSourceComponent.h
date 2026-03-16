#pragma once

#include "Component/BaseComponent.h"
#include "Component/Essential/TransformComponent.h"
#include "GLWrapper/FrameBuffer.h"

constexpr float POINT_LIGHT_FAR_PLANE = 50.0f;

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
    struct PointLightSourceData {
        glm::vec3 position;
        glm::vec3 color;

        float constant;
        float linear;
        float quadratic;

        void Bind(GL::Shader& shader, int index) const{
            std::string baseName = "pointLights[" + std::to_string(index) + "]";
            shader.SetVec3(baseName + ".position", this->position);
            shader.SetVec3(baseName + ".light", this->color);
            shader.SetFloat(baseName + ".constant", this->constant);
            shader.SetFloat(baseName + ".linear", this->linear);
            shader.SetFloat(baseName + ".quadratic", this->quadratic);
        };
    };

    PointLightSource* SetLightData(const PointLightData& newData);
    PointLightSource* SetLightData(glm::vec3 color, float linear, float quadratic, float radius);

    PointLightSourceData *GetLightData() { return &this->data; }

    void RenderShadowMap(GL::Shader& shadowShader);
    void BindShadowMap(uint8_t textureIndex) const;

    PointLightSource(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~PointLightSource() override = default;
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }
private:
    PointLightSourceData data;
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