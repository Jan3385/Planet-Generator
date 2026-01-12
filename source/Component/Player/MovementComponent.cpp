#include "MovementComponent.h"

#include "Engine/Engine.h"
#include "Engine/Input.h"

void Component::Movement::Awake()
{
    transform = this->GetOwner()->GetComponent<Component::Transform>();
    camera = this->GetOwner()->GetComponent<Component::Camera>();

    transform->SetLockPitchRange(true);
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
    if(Input::GetCursorMode() == Input::CursorMode::Trapped)
        transform->RotateBy(
            glm::vec2(-GameEngine::input->GetCursorDelta().x, -GameEngine::input->GetCursorDelta().y)
            * GameEngine::instance->DeltaTime() * 6.0f
        );

    glm::vec2 input = Input::GetMovementVector();

    glm::vec3 moveVector = glm::vec3(0.0f);
    if(input != glm::vec2(0.0f, 0.0f)){
        glm::vec3 forward = transform->GetForwardVector();
        glm::vec3 right = transform->GetRightVector();

        // ignore tilt
        forward.y = 0.0f;
        right.y = 0.0f;

        forward = glm::normalize(forward);
        right = glm::normalize(right);

        moveVector = forward * input.y + right * input.x;
        moveVector = glm::normalize(moveVector) * SPEED;
    }

    if(Input::IsKeyDown(GLFW_KEY_SPACE))
        moveVector.y += SPEED;
    if(Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
        moveVector.y -= SPEED;

    moveVector *= GameEngine::instance->DeltaTime();

    transform->MovePosBy(moveVector);

    if(GameEngine::input->IsKeyPressed(GLFW_KEY_ESCAPE))
        Input::ToggleTrappedCursor();
}

void Component::Movement::FixedUpdate()
{
}
