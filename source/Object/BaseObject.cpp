#include "BaseObject.h"

Object::BaseObject::~BaseObject()
{
    for (const auto& component : components) {
        component->OnDestroy();
    }
}

bool Object::BaseObject::HasComponentType(const std::type_index &type) const
{
    for (const auto& component : components) {
        if (typeid(*component) == type) {
            return true;
        }
    }
    
    return false;
}
bool Object::BaseObject::HasAllComponentTypes(const std::vector<std::type_index> &types) const
{
    for (const auto& type : types) {
        if (!HasComponentType(type)) return false;
    }

    return true;
}

/**
 * @brief Updates the Object
 * @details Calls Start on all components that need it and then calls Update on all updatable components
 * @note Should be only called by the engine
 */
void Object::BaseObject::Update()
{
    while(!startables.empty()){
        Component::BaseComponent* component = startables.front();
        startables.pop();

        if (!component->IsStarted()) {
            component->started = true;
            component->Start();
        }
    }

    for (auto* updatable : updatables) {
        updatable->Update();
    }
}

void Object::BaseObject::Enable()
{
    for (const auto& component : components) {
        if (!component->IsAwake()) {
            component->awake = true;
            component->Awake();
        }
        component->OnEnable();
    }

    this->enabled = true;
}

void Object::BaseObject::Disable()
{
    for (const auto& component : components) {
        component->OnDisable();
    }

    this->enabled = false;
}
