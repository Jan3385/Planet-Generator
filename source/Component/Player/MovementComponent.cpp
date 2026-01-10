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
    transform->RotateBy(glm::vec2(-GameEngine::input->GetCursorDelta().x * 0.1f, -GameEngine::input->GetCursorDelta().y * 0.1f));

    glm::vec2 input = Input::GetMovementVector();
    if(input == glm::vec2(0.0f, 0.0f)) return;

    glm::vec3 forward = transform->GetForwardVector();
    glm::vec3 right = transform->GetRightVector();

    // ignore tilt
    forward.y = 0.0f;
    right.y = 0.0f;

    forward = glm::normalize(forward);
    right = glm::normalize(right);

    glm::vec3 moveVector = forward * input.y + right * input.x;
    moveVector = glm::normalize(moveVector) * SPEED;

    transform->MovePosBy(moveVector);
}

void Component::Movement::FixedUpdate()
{
}
