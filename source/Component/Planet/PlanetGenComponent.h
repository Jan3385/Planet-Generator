#pragma once

#include "Component/BaseComponent.h"
#include "Component/Essential/TransformComponent.h"
#include "Component/Essential/Mesh/GLMeshComponent.h"

namespace Component {
/**
 * 
 */
class PlanetGen : public BaseComponent, public Component::IUpdatable {
public:
    PlanetGen(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~PlanetGen() override = default;

    void PlanetifyMesh(uint32_t seed);
protected:
    std::vector<std::type_index> GetDependencies() const override 
        { return {typeid(Component::Transform), typeid(Component::GLMesh)}; }
private:
    void Awake() override;
    void OnDestroy() override;

    void OnEnable() override;
    void OnDisable() override;

    void Update() override;
    void FixedUpdate() override;

    Transform* transform = nullptr;
    GLMesh* mesh = nullptr;

    friend class Object::BaseObject;
};
}