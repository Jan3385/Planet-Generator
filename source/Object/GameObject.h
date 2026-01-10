#pragma once

#include "Component/Essential/TransformComponent.h"
#include "Component/Essential/RenderComponent.h"
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

    Component::Transform* GetTransform();
    Component::Mesh* GetMesh();
    Component::RenderComponent* GetRenderComponent();
private:
    Component::Transform* transform = nullptr;
    Component::Mesh* mesh = nullptr;
    Component::RenderComponent* renderComponent = nullptr;
};
}