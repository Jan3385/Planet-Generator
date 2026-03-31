#pragma once

#include <chrono>

#include "Object/BaseObject.h"
#include "Object/GameObject.h"
#include "Component/Engine/CameraComponent.h"
#include "Engine/Renderer.h"
#include "Math/Color.h"

class Level {
public:
    Object::BaseObject* CreateObject();
    Object::GameObject* CreateGameObject();
    Object::BaseObject* CreateLightObject(Math::RGB color, float intensity = 1.0f);
    void ObjectDestroy(Object::BaseObject* object);

    void Update();

    void SetCamera(Component::Camera* camera) { this->mainCamera = camera; }
    Component::Camera* GetCamera() const { return mainCamera; }
    void SetSkybox(Renderer::IRendererCallback* skybox) { this->skybox = skybox; }
    Renderer::IRendererCallback* GetSkybox() const { return skybox; }

    /// @brief Maximum delta time for fixed update to avoid spiral of death
    /// @note Setting to 0 will disable clamping which can cause issues if the game freezes for a long time
    /// @warning When not 0, fixed update delta time is not guaranteed to be correct
    /// @details default 0.25f
    float fixedUpdateClamp = 0.25f;
private:
    Component::Camera* mainCamera = nullptr;
    Renderer::IRendererCallback* skybox = nullptr;

    std::vector<std::unique_ptr<Object::BaseObject>> objects;

    std::queue<Object::BaseObject*> destroyQueue;

    // Fixed update timer
    using clock = std::chrono::steady_clock;
    double accumulator = 0.0f;
    clock::time_point previousTime = clock::now();
};