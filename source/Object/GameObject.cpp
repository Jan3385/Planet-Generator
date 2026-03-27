#include "GameObject.h"

#include "Debug/Logger.h"
#include "Engine/Engine.h"

Object::GameObject::GameObject(Level* level) : BaseObject(level)
{
    this->transform = this->AddComponent<Component::Transform>();
    this->renderComponent = this->AddComponent<Component::MeshRender>()
        ->SetTransform(this->transform)
        ->SetMaterial(GameEngine::materialLibrary->GetDefaultMaterial());
}

Component::Transform *Object::GameObject::GetTransform()
{
    if(this->transform == nullptr) {
        Debug::LogError("GameObject has no Transform component!");
        return nullptr;
    }

    return this->transform;
}

Component::MeshRender *Object::GameObject::GetRenderComponent()
{
    if(this->renderComponent == nullptr) {
        Debug::LogError("GameObject has no RenderComponent component!");
        return nullptr;
    }

    return this->renderComponent;
}
