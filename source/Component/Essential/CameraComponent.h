#pragma once

#include "Component/BaseComponent.h"
#include "Component/Essential/TransformComponent.h"

namespace Component {
/**
 * 
 */
class Camera : public BaseComponent {
public:
    Camera(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~Camera() override = default;

    Camera* SetFOV(float newFov);
    Camera* SetAspectRatio(float newAspectRatio);
    Camera* SetClipPlanes(float newNearClip, float newFarClip);
    glm::mat4 GetProjection() const { return projection; }
    glm::mat4 GetView() const;
protected:
    float fov = 45.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearClip = 0.1f;
    float farClip = 1000.0f;

    void RecalculateProjection();

    glm::mat4 projection;
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform)}; }
private:
    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    Transform* transform = nullptr;

    friend class Object::BaseObject;
};
}