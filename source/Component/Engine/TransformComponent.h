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

    glm::vec3 GetPos(bool local = false);
    glm::quat GetRotQuaternion(bool local = false);

    glm::vec3 GetRot(bool local = false);

    glm::vec3 GetScale(bool local = false);

    Transform* SetPos(const glm::vec3& newPos, bool local = false);

    // dx, dy in degrees
    Transform* SetRot(const glm::vec2& newRot, bool local = false);
    // euler angles
    Transform* SetRot(const glm::vec3& newRot, bool local = false);

    Transform* SetScale(const glm::vec3& newScale, bool local = false);

    Transform* ScaleBy(const glm::vec3& deltaScale, bool local = false);

    Transform* MovePosBy(const glm::vec3& deltaPos, bool local = false);

    // dx, dy in degrees
    Transform* RotateBy(const glm::vec2& deltaRot, bool local = false);
    // euler angles
    Transform* RotateBy(const glm::vec3& deltaRot, bool local = false);

    Transform* SetUpDirection(glm::vec3 newUp);

    glm::vec3 GetUpVector() const { return modelUp; }
    glm::vec3 GetForwardVector() const { return modelForward; }
    glm::vec3 GetRightVector() const { return modelRight; }

    glm::mat4 GetMatrixTransform();

    std::vector<std::type_index> GetDependencies() const override 
        { return {}; }
protected:
    glm::quat ReconstructRotationFromEuler(const glm::vec3& euler);
    glm::quat UpdateRotationFromEuler(const glm::vec3& deltaEuler);

    void MarkDirty() { dirtyTransform = true; }
private:
    glm::vec3 position = glm::vec3(0.0f);

    glm::vec3 modelUp =      glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 modelForward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 modelRight =   glm::vec3(1.0f, 0.0f, 0.0f);

    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    glm::vec3 scale    = glm::vec3(1.0f);

    bool dirtyTransform = true;
    glm::mat4 matrixTransform;
    glm::mat4 invParentMatrixTransform = glm::inverse(glm::mat4(1.0f));
};
}