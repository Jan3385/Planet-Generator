#include "TransformComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Object/BaseObject.h"

glm::vec3 Component::Transform::GetPos(bool local)
{
    if(local) return this->position;

    glm::mat4 worldMatrix = this->GetMatrixTransform();
    return glm::vec3(worldMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

glm::quat Component::Transform::GetRotQuaternion(bool local)
{
    if(local) return this->rotation;

    glm::mat4 worldMatrix = this->GetMatrixTransform();
    return glm::quat_cast(worldMatrix);
}

/// @brief Reconstructs euler angles from the current rotation quaternion
/// @return Euler angles in radians
glm::vec3 Component::Transform::GetRot(bool local)
{
    return glm::eulerAngles(this->GetRotQuaternion(local));
}

glm::vec3 Component::Transform::GetScale(bool local)
{
    if(local) return this->scale;

    glm::mat4 worldMatrix = this->GetMatrixTransform();
    glm::vec3 scale;
    scale.x = glm::length(glm::vec3(worldMatrix[0]));
    scale.y = glm::length(glm::vec3(worldMatrix[1]));
    scale.z = glm::length(glm::vec3(worldMatrix[2]));
    return scale;
}

Component::Transform *Component::Transform::SetPos(const glm::vec3 &newPos, bool local)
{
    if(newPos == this->position && !local) return this;

    dirtyTransform = true;

    if(local)
        this->position = newPos;
    else{
        glm::vec4 localPos = this->invParentMatrixTransform * glm::vec4(newPos, 1.0f);
        this->position = glm::vec3(localPos);
    }
    
    return this;
}

Component::Transform* Component::Transform::SetRot(const glm::vec2 &newRot, bool local)
{
    dirtyTransform = true;

    if(local)
        this->rotation = ReconstructRotationFromEuler(glm::vec3(newRot.x, newRot.y, 0.0f));
    else{
        glm::vec4 localRot = this->invParentMatrixTransform * glm::vec4(newRot.x, newRot.y, 0.0f, 1.0f);
        this->rotation = ReconstructRotationFromEuler(glm::vec3(localRot.x, localRot.y, 0.0f));
    }

    return this;
}

Component::Transform* Component::Transform::SetRot(const glm::vec3 &newRot, bool local)
{
    dirtyTransform = true;
    
    if(local)
        this->rotation = ReconstructRotationFromEuler(newRot);
    else{
        glm::vec4 localRot = this->invParentMatrixTransform * glm::vec4(newRot, 1.0f);
        this->rotation = ReconstructRotationFromEuler(glm::vec3(localRot.x, localRot.y, localRot.z));
    }

    return this;
}

Component::Transform* Component::Transform::RotateBy(const glm::vec2 &deltaRot, bool local)
{
    if(deltaRot == glm::vec2(0.0f)) return this;
    
    this->dirtyTransform = true;
    
    if(local)
        this->rotation = UpdateRotationFromEuler(glm::vec3(deltaRot.x, deltaRot.y, 0.0f));
    else{
        glm::vec4 localRot = this->invParentMatrixTransform * glm::vec4(deltaRot.x, deltaRot.y, 0.0f, 1.0f);
        this->rotation = UpdateRotationFromEuler(glm::vec3(localRot.x, localRot.y, localRot.z));
    }

    return this;
}

Component::Transform* Component::Transform::RotateBy(const glm::vec3 &deltaRot, bool local)
{
    if(deltaRot == glm::vec3(0.0f)) return this;

    dirtyTransform = true;

    if(local)
        this->rotation = UpdateRotationFromEuler(deltaRot);
    else{
        glm::vec4 localRot = this->invParentMatrixTransform * glm::vec4(deltaRot.x, deltaRot.y, deltaRot.z, 1.0f);
        this->rotation = UpdateRotationFromEuler(glm::vec3(localRot.x, localRot.y, localRot.z));
    }

    return this;
}

Component::Transform* Component::Transform::SetScale(const glm::vec3 &newScale, bool local)
{
    dirtyTransform = true;

    if(local)
        this->scale = newScale;
    else{
        glm::vec4 localScale = this->invParentMatrixTransform * glm::vec4(newScale, 1.0f);
        this->scale = glm::vec3(localScale);
    }

    return this;
}

Component::Transform *Component::Transform::ScaleBy(const glm::vec3 &deltaScale, bool local)
{
    if(deltaScale == glm::vec3(0.0f)) return this;

    dirtyTransform = true;

    if(local)
        this->scale += deltaScale;
    else{
        glm::vec4 localScale = this->invParentMatrixTransform * glm::vec4(deltaScale, 1.0f);
        this->scale += glm::vec3(localScale);
    }

    return this;
}

Component::Transform* Component::Transform::MovePosBy(const glm::vec3 &deltaPos, bool local)
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

glm::mat4 Component::Transform::GetMatrixTransform()
{
    if(!dirtyTransform) return matrixTransform;

    this->matrixTransform = glm::mat4(1.0f);
    this->matrixTransform = glm::translate(this->matrixTransform, this->position);
    this->matrixTransform = this->matrixTransform * glm::mat4_cast(this->rotation);
    this->matrixTransform = glm::scale(this->matrixTransform, this->scale);

    // apply parent transforms
    Object::BaseObject* parent = this->GetOwner()->GetParent();
    if(parent){
        Component::Transform* parentTransform = parent->GetComponent<Component::Transform>();
        if(parentTransform) {
            this->invParentMatrixTransform = glm::inverse(parentTransform->GetMatrixTransform());
            this->matrixTransform = parentTransform->GetMatrixTransform() * this->matrixTransform;
        }
    }

    std::vector<Object::BaseObject*> children = this->GetOwner()->GetChildren();
    for(Object::BaseObject* child : children){
        Component::Transform* childTransform = child->GetComponent<Component::Transform>();
        if(childTransform){
            childTransform->MarkDirty();
        }
    }

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
