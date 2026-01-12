#include "BaseObject.h"

#include "Component/Essential/TransformComponent.h"
#include "Debug/Logger.h"

using namespace Object;

template<class ComponentType>
ComponentType* BaseObject::AddComponent()
{
    static_assert(std::is_base_of<Component::BaseComponent, ComponentType>::value, "ComponentType must be derived from BaseComponent");

    auto component = std::make_unique<ComponentType>(this);
    ComponentType* componentPtr = component.get();

    components.push_back(std::move(component));

    if constexpr (std::is_base_of_v<Component::IUpdatable, ComponentType>) {
        updatables.push_back(componentPtr);
    }

    componentPtr->awake = true;
    componentPtr->Awake();

    componentPtr->CheckDependencies();

    return componentPtr;
}

template <class ComponentType>
inline ComponentType *Object::BaseObject::GetComponent()
{
    for(const auto& component : components) {
        if(auto casted = dynamic_cast<ComponentType*>(component.get())) {
            return casted;
        }
    }

    return nullptr;
}

template <class ComponentType>
inline std::vector<ComponentType *> Object::BaseObject::GetComponents()
{
    std::vector<ComponentType*> foundComponents;

    for(const auto& component : components) {
        if(auto casted = dynamic_cast<ComponentType*>(component.get())) {
            foundComponents.push_back(casted);
        }
    }

    return foundComponents;
}

template <class ComponentType>
inline void Object::BaseObject::RemoveComponents()
{
    if constexpr (std::is_same_v<ComponentType, Component::Transform>) {
        Debug::LogWarn("Removed Transform component from object. This may lead to unexpected behavior!");
    }

    if constexpr (std::is_base_of_v<Component::IUpdatable, ComponentType>) {
        std::erase_if(updatables, [](Component::IUpdatable* updatable) {
            return typeid(*updatable) == typeid(ComponentType);
        });
    }

    std::erase_if(components, [](const std::unique_ptr<Component::BaseComponent>& component) {
        return typeid(*component) == typeid(ComponentType);
    });
}
