#include "MovementComponent.h"

#include "Engine/Engine.h"
#include "Engine/Input.h"

void Component::Movement::Awake()
{
    transform = this->GetOwner()->GetComponent<Component::Transform>();
    camera = this->GetOwner()->GetComponent<Component::Camera>();
    
    if(!transform) Debug::LogFatal("Movement component requires a Transform component to function properly!");
    if(!camera) Debug::LogFatal("Movement component requires a Camera component to function properly!");
}

void Component::Movement::OnDestroy()
{
}

void Component::Movement::OnEnable()
{
}

void Component::Movement::OnDisable()
{
}

void Component::Movement::Update()
{
    glm::vec2 movementVector = Input::GetMovementVector();
    movementVector *= SPEED;

    transform->MovePosBy(glm::vec3(movementVector.x, 0.0f, movementVector.y));

    transform->RotateBy(glm::vec3(-GameEngine::input->GetCursorDelta().y * 0.1f, -GameEngine::input->GetCursorDelta().x * 0.1f, 0.0f));
}

void Component::Movement::FixedUpdate()
{
}
