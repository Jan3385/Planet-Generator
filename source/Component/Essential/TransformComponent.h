#pragma once

#include <glm/glm.hpp>

#include "Component/BaseComponent.h"

namespace Component {
/**
 * @brief Transform component class
 * @details Handles position, rotation and scale of the object
 * @note Once inserted into an object, it should not be removed
 */
class Transform : public BaseComponent {
public:

    Transform(Object::BaseObject* owner) : BaseComponent(owner) {};
    ~Transform() override = default;

    glm::vec3 GetPos() const { return position; }
    glm::vec3 GetRot() const { return rotation; }
    glm::vec3 GetScale() const { return scale; }
    void SetPos(const glm::vec3& newPos);
    void SetRot(const glm::vec3& newRot);
    void SetScale(const glm::vec3& newScale);
    void MovePosBy(const glm::vec3& deltaPos);
    void RotateBy(const glm::vec3& deltaRot);
    void ScaleBy(const glm::vec3& deltaScale);

    glm::mat4 GetMatrixTransform();
private:
    glm::vec3 position = glm::vec3(0.0f);
    // Euler angles in degrees
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale    = glm::vec3(1.0f);

    bool dirtyTransform = true;
    glm::mat4 matrixTransform;
};
}