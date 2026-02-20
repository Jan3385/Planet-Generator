#include "TransformComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

Component::Transform* Component::Transform::SetPos(const glm::vec3 &newPos)
{
    if(newPos == this->position) return this;

    dirtyTransform = true;
    this->position = newPos;
    
    return this;
}

Component::Transform* Component::Transform::SetRot(const glm::vec2 &newRot)
{
    dirtyTransform = true;

    this->rotation = ReconstructRotationFromEuler(glm::vec3(newRot.y, newRot.x, 0.0f));

    return this;
}

Component::Transform* Component::Transform::SetRot(const glm::vec3 &newRot)
{
    dirtyTransform = true;
    
    this->rotation = ReconstructRotationFromEuler(newRot);

    return this;
}

Component::Transform* Component::Transform::RotateBy(const glm::vec2 &deltaRot)
{
    if(deltaRot == glm::vec2(0.0f)) return this;
    
    this->dirtyTransform = true;
    
    this->rotation = UpdateRotationFromEuler(glm::vec3(deltaRot.x, deltaRot.y, 0.0f));

    return this;
}

Component::Transform* Component::Transform::RotateBy(const glm::vec3 &deltaRot)
{
    if(deltaRot == glm::vec3(0.0f)) return this;

    dirtyTransform = true;

    this->rotation = UpdateRotationFromEuler(deltaRot);

    return this;
}

Component::Transform* Component::Transform::SetScale(const glm::vec3 &newScale)
{
    dirtyTransform = true;
    this->scale = newScale;

    return this;
}

Component::Transform* Component::Transform::MovePosBy(const glm::vec3 &deltaPos)
{
    if(deltaPos == glm::vec3(0.0f)) return this;

    dirtyTransform = true;
    this->position += deltaPos;

    return this;
}

Component::Transform* Component::Transform::SetUpDirection(glm::vec3 newUp)
{
    newUp = glm::normalize(newUp);
    if(this->modelUp == newUp) return this;

    dirtyTransform = true;

    glm::vec3 oldUp = this->modelUp;
    this->modelUp = newUp;

    glm::quat rot = glm::rotation(oldUp, newUp);
    this->rotation = glm::normalize(rot * this->rotation);

    this->modelForward = this->rotation * glm::vec3(0.0f, 0.0f, -1.0f);
    this->modelRight = this->rotation * glm::vec3(1.0f, 0.0f, 0.0f);

    return this;
}

Component::Transform* Component::Transform::ScaleBy(const glm::vec3 &deltaScale)
{
    if(deltaScale == glm::vec3(0.0f)) return this;

    dirtyTransform = true;
    this->scale += deltaScale;

    return this;
}

glm::mat4 Component::Transform::GetMatrixTransform()
{
    if(!dirtyTransform) return matrixTransform;

    this->matrixTransform = glm::mat4(1.0f);
    this->matrixTransform = glm::translate(this->matrixTransform, this->position);
    this->matrixTransform = this->matrixTransform * glm::mat4_cast(this->rotation);
    this->matrixTransform = glm::scale(this->matrixTransform, this->scale);

    dirtyTransform = false;

    return this->matrixTransform;
}

glm::quat Component::Transform::ReconstructRotationFromEuler(const glm::vec3 &euler)
{
    glm::quat q = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    glm::vec3 up = glm::normalize(this->modelUp);
    q = glm::angleAxis(glm::radians(euler.x), up) * q;

    glm::vec3 right = glm::normalize(glm::cross(up, q * glm::vec3(0.0f, 0.0f, 1.0f)));
    q = glm::angleAxis(glm::radians(euler.y), right) * q;

    glm::vec3 forward = glm::normalize(q * glm::vec3(0.0f, 0.0f, -1.0f));
    q = glm::angleAxis(glm::radians(euler.z), forward) * q;

    q = glm::normalize(q);

    this->modelForward = q * glm::vec3(0.0f, 0.0f, -1.0f);
    this->modelRight = q * glm::vec3(1.0f, 0.0f, 0.0f);

    return q;
}

glm::quat Component::Transform::UpdateRotationFromEuler(const glm::vec3 &deltaEuler)
{
    glm::quat q = this->rotation;

    q = glm::angleAxis(glm::radians(deltaEuler.x), this->modelUp) * q;

    glm::vec3 right = glm::normalize(q * glm::vec3(1.0f, 0.0f, 0.0f));
    q = glm::angleAxis(glm::radians(deltaEuler.y), right) * q;

    glm::vec3 forward = q * glm::vec3(0.0f, 0.0f, -1.0f);
    q = glm::angleAxis(glm::radians(deltaEuler.z), forward) * q;

    q = glm::normalize(q);

    this->modelForward = q * glm::vec3(0.0f, 0.0f, -1.0f);
    this->modelRight   = q * glm::vec3(1.0f, 0.0f, 0.0f);

    return q;
}
