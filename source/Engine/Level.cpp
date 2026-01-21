#include "Level.h"

#include "Generator/MeshGenerator.h"
#include "Component/Essential/Renderer/ColorMeshRenderComponent.h"
#include "Component/Essential/PointLightSourceComponent.h"
#include <memory>

Object::BaseObject *Level::CreateObject()
{
    auto object = std::make_unique<Object::BaseObject>();

    Object::BaseObject* objectPtr = object.get();
    objects.push_back(std::move(object));

    return objectPtr;
}

Object::GameObject *Level::CreateGameObject()
{
    auto gameObject = std::make_unique<Object::GameObject>();

    Object::GameObject* gameObjectPtr = gameObject.get();
    objects.push_back(std::move(gameObject));

    return gameObjectPtr;
}

Object::BaseObject *Level::CreateLightObject(Math::RGB color)
{
    auto lightObject = std::make_unique<Object::BaseObject>();

    Component::Transform *lightTransform = lightObject->AddComponent<Component::Transform>();

    lightTransform->SetScale(glm::vec3(0.2f));

    Component::ColorMeshRender *lightRenderer = lightObject->AddComponent<Component::ColorMeshRender>();
        
    lightRenderer->color = glm::vec3(color.ToVec3());

    Component::PointLightSource *pointLight = lightObject->AddComponent<Component::PointLightSource>();
    pointLight->SetLightData(lightRenderer->color, 0.5f, 0.5f, 0.35f, 0.44f, 5.0f);
    
    Object::BaseObject* lightObjectPtr = lightObject.get();
    objects.push_back(std::move(lightObject));

    return lightObjectPtr;
}

void Level::ObjectDestroy(Object::BaseObject *object)
{
    destroyQueue.push(object);
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

    for(const auto& object : objects) {
        if(object->IsEnabled()) {
            object->Update();
        }
    }
}
