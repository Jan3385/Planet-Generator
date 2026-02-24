#include "BaseObject.h"

#include "Engine/Level.h"

Object::BaseObject::~BaseObject()
{
    for (const auto& child : children) {
        if(child && child->level){
            child->SetParent(nullptr);
        }
    }

    if(this->parent)
        this->parent->RemoveChild(this);
    this->parent = nullptr;

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

/// @brief Sets the parent of this object
/// @param newParent pointer to the new parent, nullptr to make root object
void Object::BaseObject::SetParent(BaseObject *newParent)
{
    if(this->parent){
        this->parent->RemoveChild(this);
    }
    this->parent = newParent;

    if(this->parent)   
        this->parent->AddChild(this);
}

std::vector<BaseObject *> Object::BaseObject::GetChildren() const
{
    return this->children;
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

void Object::BaseObject::AddChild(BaseObject* child)
{
    this->children.push_back(child);
}

void Object::BaseObject::RemoveChild(BaseObject *child)
{
    auto it = std::find(children.begin(), children.end(), child);

    if (it != children.end()) children.erase(it);
}
