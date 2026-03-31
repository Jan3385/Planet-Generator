#include "Level.h"

#include <memory>

#include "Engine/Engine.h"
#include "Generator/MeshGenerator.h"
#include "Component/Engine/PointLightSourceComponent.h"

Object::BaseObject *Level::CreateObject()
{
    auto object = std::make_unique<Object::BaseObject>(this);

    Object::BaseObject* objectPtr = object.get();
    objects.push_back(std::move(object));

    return objectPtr;
}

Object::GameObject *Level::CreateGameObject()
{
    auto gameObject = std::make_unique<Object::GameObject>(this);

    Object::GameObject* gameObjectPtr = gameObject.get();
    objects.push_back(std::move(gameObject));

    return gameObjectPtr;
}

Object::BaseObject *Level::CreateLightObject(Math::RGB color, float intensity)
{
    auto lightObject = std::make_unique<Object::BaseObject>(this);

    Component::Transform *lightTransform = lightObject->AddComponent<Component::Transform>();

    lightTransform->SetScale(glm::vec3(0.2f));

    lightObject->AddComponent<Component::MeshRender>()
        ->SetTransform(lightTransform);

    Component::PointLightSource *pointLight = lightObject->AddComponent<Component::PointLightSource>();
    
    Component::PointLightSource::PointLightData lightData;
    lightData.color = glm::vec3(color.ToVec3()) * intensity;

    pointLight->SetLightData(lightData);
    
    Object::BaseObject* lightObjectPtr = lightObject.get();
    objects.push_back(std::move(lightObject));

    return lightObjectPtr;
}

void Level::ObjectDestroy(Object::BaseObject *object)
{    
    destroyQueue.push(object);

    for (const auto& child : object->GetChildren())
        if(child) [[likely]] destroyQueue.push(child);
}

/**
 * @brief Updates the Level
 * @details Destroys queued objects and updates all enabled objects
 * @note Should be only called by the engine
 */
void Level::Update()
{
    while(!destroyQueue.empty()){
        Object::BaseObject* object = destroyQueue.front();
        destroyQueue.pop();

        std::erase_if(this->objects, [object](const std::unique_ptr<Object::BaseObject>& objPtr) {
            return objPtr.get() == object;
        });
    }

    // Early Update
    for(const auto& object : objects) {
        if(object->IsEnabled()) {
            object->EarlyUpdate();
        }
    }

    // Update
    for(const auto& object : objects) {
        if(object->IsEnabled()) {
            object->Update();
        }
    }

    // Fixed Update
    clock::time_point currentTime = clock::now();
    double deltaTime = std::chrono::duration<double>(currentTime - previousTime).count();
    previousTime = currentTime;
    
    // Clamping to avoid spiral of death
    if (this->fixedUpdateClamp > 0.0f && deltaTime > this->fixedUpdateClamp) deltaTime = this->fixedUpdateClamp;
    accumulator += deltaTime;

    while (accumulator >= GameEngine::FIXED_UPDATE_INTERVAL) {
        for(const auto& object : objects) {
            if(object->IsEnabled()) {
                object->FixedUpdate();
            }
        }
        accumulator -= GameEngine::FIXED_UPDATE_INTERVAL;
    }

    // Late Update
    for(const auto& object : objects) {
        if(object->IsEnabled()) {
            object->LateUpdate();
        }
    }
}
