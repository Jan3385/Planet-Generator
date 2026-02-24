#include "GameObject.h"

#include "Debug/Logger.h"

Object::GameObject::GameObject(Level* level) : BaseObject(level)
{
    this->transform = this->AddComponent<Component::Transform>();
    this->renderComponent = this->AddComponent<Component::PhongMeshRender>();
}

Component::Transform *Object::GameObject::GetTransform()
{
    if(this->transform == nullptr) {
        Debug::LogError("GameObject has no Transform component!");
        return nullptr;
    }

    return this->transform;
}

Component::PhongMeshRender *Object::GameObject::GetRenderComponent()
{
    if(this->renderComponent == nullptr) {
        Debug::LogError("GameObject has no RenderComponent component!");
        return nullptr;
    }

    return this->renderComponent;
}
