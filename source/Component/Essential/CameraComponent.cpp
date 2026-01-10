#include "CameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Object/BaseObject.h"
#include "Engine/Engine.h"
#include "Debug/Logger.h"

void Component::Camera::SetFOV(float newFov)
{
    if(this->fov == newFov)
        return;

    this->fov = newFov;
    this->RecalculateProjection();
}

void Component::Camera::SetAspectRatio(float newAspectRatio)
{
    if(this->aspectRatio == newAspectRatio)
        return;

    this->aspectRatio = newAspectRatio;
    this->RecalculateProjection();
}

void Component::Camera::SetClipPlanes(float newNearClip, float newFarClip)
{
    if(this->nearClip == newNearClip && this->farClip == newFarClip)
        return;

    this->nearClip = newNearClip;
    this->farClip = newFarClip;
    this->RecalculateProjection();
}

glm::mat4 Component::Camera::GetView() const
{
    glm::mat4 view = glm::mat4(1.0f);
    view = view * glm::mat4_cast(glm::conjugate(this->transform->GetRotQuaternion()));
    view = glm::translate(view, -this->transform->GetPos());
    return view;
}

void Component::Camera::RecalculateProjection()
{
    this->projection = glm::perspective(glm::radians(this->fov), this->aspectRatio, this->nearClip, this->farClip);
}

void Component::Camera::Awake()
{
    if(!this->transform)
        this->transform = this->GetOwner()->GetComponent<Component::Transform>();

    if(!this->transform)
        Debug::LogFatal("Camera component requires a Transform component to function properly!");

    GameEngine::currentLevel->SetCamera(this);
}

void Component::Camera::OnDestroy()
{
    GameEngine::currentLevel->SetCamera(nullptr);
}

void Component::Camera::OnEnable()
{
    GameEngine::currentLevel->SetCamera(this);
}

void Component::Camera::OnDisable()
{
    if(GameEngine::currentLevel->GetCamera() == this)
        GameEngine::currentLevel->SetCamera(nullptr);
}
