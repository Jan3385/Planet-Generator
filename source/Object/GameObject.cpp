#include "GameObject.h"

#include "Debug/Logger.h"

Object::GameObject::GameObject()
{
    this->transform = this->AddComponent<Component::Transform>();
    this->mesh = this->AddComponent<Component::Mesh>();
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

Component::Mesh *Object::GameObject::GetMesh()
{
    if(this->mesh == nullptr) {
        Debug::LogError("GameObject has no Mesh component!");
        return nullptr;
    }

    return this->mesh;
}

Component::PhongMeshRender *Object::GameObject::GetRenderComponent()
{
    if(this->renderComponent == nullptr) {
        Debug::LogError("GameObject has no RenderComponent component!");
        return nullptr;
    }

    return this->renderComponent;
}
