#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

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
    glm::quat GetRotQuaternion() const { return rotation; }
    glm::vec3 GetRot() const { return glm::eulerAngles(rotation); }
    
    glm::vec3 GetForwardVector() const { return rotation * glm::vec3(0.0f, 0.0f, -1.0f); }
    glm::vec3 GetRightVector() const { return rotation * glm::vec3(1.0f, 0.0f, 0.0f); }
    glm::vec3 GetUpVector() const { return rotation * glm::vec3(0.0f, 1.0f, 0.0f); }

    glm::vec3 GetScale() const { return scale; }

    Transform* SetPos(const glm::vec3& newPos);

    // dx, dy in degrees
    Transform* SetRot(const glm::vec2& newRot);
    // euler angles
    Transform* SetRot(const glm::vec3& newRot);

    Transform* SetScale(const glm::vec3& newScale);

    Transform* MovePosBy(const glm::vec3& deltaPos);

    // dx, dy in degrees
    Transform* RotateBy(const glm::vec2& deltaRot);
    // euler angles
    Transform* RotateBy(const glm::vec3& deltaRot);

    /**
     * @brief Sets whether to lock the pitch rotation between -89 and 89 degrees
     * @param lock True to lock, false to unlock
     * @note Default `false`
     */
    Transform* SetLockPitchRange(bool lock) { this->lockPitchRange = lock; return this; }

    Transform* ScaleBy(const glm::vec3& deltaScale);

    glm::mat4 GetMatrixTransform();

    std::vector<std::type_index> GetDependencies() const override 
        { return {}; }
private:
    glm::vec3 position = glm::vec3(0.0f);

    bool lockPitchRange = false;

    // quaternion angle
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;

    glm::vec3 scale    = glm::vec3(1.0f);

    bool dirtyTransform = true;
    glm::mat4 matrixTransform;
};
}