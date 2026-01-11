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
    };

    void SetLightData(const PointLightData& newData);
    void SetLightData(glm::vec3 color, float intensity, float radius);
    PointLightData GetLightData() const;

    PointLightSource(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~PointLightSource() override = default;
protected:
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