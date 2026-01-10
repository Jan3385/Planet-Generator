#pragma once

#include "Object/BaseObject.h"
#include "Object/GameObject.h"

class Level {
public:
    Object::BaseObject* CreateObject();
    Object::GameObject* CreateGameObject();
    void ObjectDestroy(Object::BaseObject* object);

    void Update();
private:
    std::vector<std::unique_ptr<Object::BaseObject>> objects;

    std::queue<Object::BaseObject*> destroyQueue;
};