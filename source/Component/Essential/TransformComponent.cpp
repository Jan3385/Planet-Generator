#include "TransformComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

void Component::Transform::SetPos(const glm::vec3 &newPos)
{
    if(newPos == this->position) return;

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
    
    this->rotation = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), glm::radians(roll));
}

void Component::Transform::SetRot(const glm::vec3 &newRot)
{
    dirtyTransform = true;
    this->yaw = newRot.y;
    this->pitch = newRot.x;
    if(this->lockPitchRange) this->pitch = glm::clamp(pitch, -89.0f, 89.0f);
    this->roll = newRot.z;

    this->rotation = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), glm::radians(roll));
}

void Component::Transform::SetScale(const glm::vec3 &newScale)
{
    dirtyTransform = true;
    this->scale = newScale;
}

void Component::Transform::MovePosBy(const glm::vec3 &deltaPos)
{
    if(deltaPos == glm::vec3(0.0f)) return;

    dirtyTransform = true;
    this->position += deltaPos;
}

void Component::Transform::RotateBy(const glm::vec2 &deltaRot)
{
    if(deltaRot == glm::vec2(0.0f)) return;

    dirtyTransform = true;
    this->yaw += deltaRot.x;
    this->pitch += deltaRot.y;
    if(this->lockPitchRange) this->pitch = glm::clamp(pitch, -89.0f, 89.0f);
    this->roll = 0.0f;
    
    this->rotation = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), glm::radians(roll));
}

void Component::Transform::RotateBy(const glm::vec3 &deltaRot)
{
    if(deltaRot == glm::vec3(0.0f)) return;

    dirtyTransform = true;
    this->yaw += deltaRot.y;
    this->pitch += deltaRot.x;
    if(this->lockPitchRange) this->pitch = glm::clamp(pitch, -89.0f, 89.0f);
    this->roll += deltaRot.z;
    this->rotation = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), glm::radians(roll));
}

void Component::Transform::ScaleBy(const glm::vec3 &deltaScale)
{
    if(deltaScale == glm::vec3(0.0f)) return;

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
