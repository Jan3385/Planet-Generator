#pragma once

#include <vector>
#include <memory>
#include <queue>
#include <typeindex>

#include "Component/BaseComponent.h"

namespace Object {
class BaseObject {
public:
    BaseObject() = default;
    virtual ~BaseObject();

    // Disable copy and move
    BaseObject(const BaseObject&) = delete;
    BaseObject& operator=(const BaseObject&) = delete;
    BaseObject(BaseObject&&) = delete;
    BaseObject& operator=(BaseObject&&) = delete;

    template<class ComponentType>
    ComponentType* AddComponent();

    template<class ComponentType>
    ComponentType* GetComponent();

    template<class ComponentType>
    std::vector<ComponentType*> GetComponents();

    bool HasComponentType(const std::type_index &type) const;
    bool HasAllComponentTypes(const std::vector<std::type_index> &types) const;

    template<class ComponentType>
    void RemoveComponents();

    void Update();

    void Enable();
    void Disable();
    bool IsEnabled() const { return enabled; }
private:
    bool enabled = true;

    std::vector<std::unique_ptr<Component::BaseComponent>> components;
    std::vector<Component::IUpdatable*> updatables;

    std::queue<Component::BaseComponent*> startables;
};
}
#include "BaseObject.inl"