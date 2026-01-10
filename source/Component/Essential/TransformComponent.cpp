#include "TransformComponent.h"

#include <glm/gtc/matrix_transform.hpp>

void Component::Transform::SetPos(const glm::vec3 &newPos)
{
    dirtyTransform = true;
    this->position = newPos;
}

void Component::Transform::SetRot(const glm::vec2 &newRot)
{
    dirtyTransform = true;
    this->yaw = newRot.x;
    this->pitch = newRot.y;
    if(this->lockPitchRange) this->pitch = glm::clamp(pitch, -89.0f, 89.0f);
    this->roll = 0.0f;
    this->rotation = glm::quat(glm::radians(glm::vec3(pitch, yaw, roll)));
}

void Component::Transform::SetRot(const glm::vec3 &newRot)
{
    dirtyTransform = true;
    this->yaw = newRot.x;
    this->pitch = newRot.y;
    if(this->lockPitchRange) this->pitch = glm::clamp(pitch, -89.0f, 89.0f);
    this->roll = newRot.z;
    this->rotation = glm::quat(glm::radians(glm::vec3(pitch, yaw, roll)));
}

void Component::Transform::SetScale(const glm::vec3 &newScale)
{
    dirtyTransform = true;
    this->scale = newScale;
}

void Component::Transform::MovePosBy(const glm::vec3 &deltaPos)
{
    dirtyTransform = true;
    this->position += deltaPos;
}

void Component::Transform::RotateBy(const glm::vec2 &deltaRot)
{
    dirtyTransform = true;
    this->yaw += deltaRot.x;
    this->pitch += deltaRot.y;
    if(this->lockPitchRange) this->pitch = glm::clamp(pitch, -89.0f, 89.0f);
    this->roll = 0.0f;
    this->rotation = glm::quat(glm::radians(glm::vec3(pitch, yaw, roll)));
}

void Component::Transform::RotateBy(const glm::vec3 &deltaRot)
{
    dirtyTransform = true;
    this->yaw += deltaRot.x;
    this->pitch += deltaRot.y;
    if(this->lockPitchRange) this->pitch = glm::clamp(pitch, -89.0f, 89.0f);
    this->roll += deltaRot.z;
    this->rotation = glm::quat(glm::radians(glm::vec3(pitch, yaw, roll)));
}

void Component::Transform::ScaleBy(const glm::vec3 &deltaScale)
{
    dirtyTransform = true;
    this->scale += deltaScale;
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
