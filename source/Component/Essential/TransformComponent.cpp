#include "TransformComponent.h"

#include <glm/gtc/matrix_transform.hpp>

void Component::Transform::SetPos(const glm::vec3 &newPos)
{
    dirtyTransform = true;
    this->position = newPos;
}

void Component::Transform::SetRot(const glm::vec3 &newRot)
{
    dirtyTransform = true;
    this->rotation = newRot;
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

void Component::Transform::RotateBy(const glm::vec3 &deltaRot)
{
    dirtyTransform = true;
    this->rotation += deltaRot;
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
    this->matrixTransform = glm::rotate(this->matrixTransform, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    this->matrixTransform = glm::rotate(this->matrixTransform, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    this->matrixTransform = glm::rotate(this->matrixTransform, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    this->matrixTransform = glm::scale(this->matrixTransform, this->scale);

    dirtyTransform = false;

    return this->matrixTransform;
}
