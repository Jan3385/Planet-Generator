#pragma once

#include "Object/BaseObject.h"
#include "Object/GameObject.h"
#include "Component/Essential/CameraComponent.h"

class Level {
public:
    Object::BaseObject* CreateObject();
    Object::GameObject* CreateGameObject();
    void ObjectDestroy(Object::BaseObject* object);

    void Update();

    void SetCamera(Component::Camera* camera) { this->mainCamera = camera; }
    Component::Camera* GetCamera() const { return mainCamera; }
private:
    Component::Camera* mainCamera = nullptr;

    std::vector<std::unique_ptr<Object::BaseObject>> objects;

    std::queue<Object::BaseObject*> destroyQueue;
};