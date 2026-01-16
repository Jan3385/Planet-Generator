#pragma once

#include "Component/Essential/TransformComponent.h"
#include "Component/Essential/Renderer/PhongMeshRenderComponent.h"
#include "Object/BaseObject.h"

/**
 * @brief Game object class
 * @details Inherits from BaseObject and adds basic game object functionality
 */
namespace Object {
class GameObject : public BaseObject {
public:
    GameObject();
    ~GameObject() override = default;

    // disable copy
    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    // enable move
    GameObject(GameObject&& other){
        this->transform = other.transform;
        this->mesh = other.mesh;
        this->renderComponent = other.renderComponent;
    };
    GameObject& operator=(GameObject&& other){
        this->transform = other.transform;
        this->mesh = other.mesh;
        this->renderComponent = other.renderComponent;
        return *this;
    };

    Component::Transform* GetTransform();
    Component::GLMesh* GetMesh();
    Component::PhongMeshRender* GetRenderComponent();
private:
    Component::Transform* transform = nullptr;
    Component::GLMesh* mesh = nullptr;
    Component::PhongMeshRender* renderComponent = nullptr;
};
}