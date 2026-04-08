#pragma once

#include "Component/BaseComponent.h"
#include "Component/Engine/TransformComponent.h"

namespace Component {
/**
 * 
 */
class GravityGenerator : public BaseComponent{
public:
    GravityGenerator(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~GravityGenerator() override = default;

    static glm::vec3 GetGravityAt(const glm::vec3& point);

    float strength = 9.81f;

    GravityGenerator* SetTransform(Component::Transform* transform) {
        this->transform = transform;
        return this;
    }
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform) }; }

    glm::vec3 GetPos() const;
private:
    Component::Transform* transform = nullptr;
    static std::vector<GravityGenerator*> gravitySourceInstances;

    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    friend class Object::BaseObject;
};
}