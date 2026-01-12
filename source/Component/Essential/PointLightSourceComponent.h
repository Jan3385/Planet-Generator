#pragma once

#include "Component/BaseComponent.h"
#include "Component/Essential/TransformComponent.h"
#include "Engine/Lighting.h"

namespace Component {
/**
 * 
 */
class PointLightSource : public BaseComponent, public IUpdatable {
public:
    struct PointLightData{
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float intensity = 1.0f;
        float radius = 5.0f;

        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    };

    void SetLightData(const PointLightData& newData);
    void SetLightData(glm::vec3 color, float diffuseIntensity, float specularIntensity, float linear, float quadratic, float radius);

    PointLightSource(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~PointLightSource() override = default;
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }
private:
    Lighting::PointLightSource data;

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