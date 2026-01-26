#pragma once

#include "Component/BaseComponent.h"
#include "Component/Essential/TransformComponent.h"
#include "Component/Essential/CameraComponent.h"

namespace Component {
/**
 * 
 */
class Movement : public BaseComponent, public Component::IUpdatable, public Component::IImGuiUpdatable {
public:
    Movement(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~Movement() override = default;
protected:
    float speed = 4.8f;

    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Camera), typeid(Component::Transform)}; }
private:
    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    void Update() override;
    void FixedUpdate() override;

    void ImGuiUpdate() override;

    Transform* transform = nullptr;
    Camera* camera = nullptr;

    friend class Object::BaseObject;
};
}