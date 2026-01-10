#include "Level.h"

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
