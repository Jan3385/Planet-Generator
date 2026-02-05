#pragma once

#include "Object/BaseObject.h"
#include "Object/GameObject.h"
#include "Component/Essential/CameraComponent.h"
#include "Component/Essential/Renderer/SkyboxRenderComponent.h"
#include "Math/Color.h"

class Level {
public:
    Object::BaseObject* CreateObject();
    Object::GameObject* CreateGameObject();
    Object::BaseObject* CreateLightObject(Math::RGB color);
    void ObjectDestroy(Object::BaseObject* object);

    void Update();

    void SetCamera(Component::Camera* camera) { this->mainCamera = camera; }
    Component::Camera* GetCamera() const { return mainCamera; }
    void SetSkybox(Component::SkyboxRender* skybox) { this->skybox = skybox; }
    Component::SkyboxRender* GetSkybox() const { return skybox; }
private:
    Component::Camera* mainCamera = nullptr;
    Component::SkyboxRender* skybox = nullptr;

    std::vector<std::unique_ptr<Object::BaseObject>> objects;

    std::queue<Object::BaseObject*> destroyQueue;
};