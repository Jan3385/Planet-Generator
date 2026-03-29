#include "MovementComponent.h"

#include "Engine/Engine.h"
#include "Engine/Input.h"

void Component::Movement::Awake()
{
    transform = this->GetOwner()->GetComponent<Component::Transform>();
    camera = this->GetOwner()->GetComponent<Component::Camera>();
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
    if(this->orientTowardsGravity){
        glm::vec3 gravDir = glm::normalize(nearestPlanetPos - transform->GetPos());
        glm::vec3 upDir = -gravDir;
        transform->SetUpDirection(upDir);
    } else{
        transform->SetUpDirection(glm::vec3(0.0f, 1.0f, 0.0f));
    }
    glm::vec3 upVec = transform->GetUpVector();

    if(Input::GetCursorMode() == Input::CursorMode::Trapped){
        transform->RotateBy(
            glm::vec2(-GameEngine::input->GetCursorDelta().x, -GameEngine::input->GetCursorDelta().y)
            * 0.15f
        );
    }
    glm::vec2 input = Input::GetMovementVector();


    glm::vec3 moveVector = glm::vec3(0.0f);
    if(input != glm::vec2(0.0f, 0.0f)){
        glm::vec3 forward = transform->GetForwardVector();
        glm::vec3 right = transform->GetRightVector();

        forward = glm::normalize(forward);
        right = glm::normalize(right);

        moveVector = forward * input.y + right * input.x;
        
        // project to a plane
        moveVector = moveVector - glm::dot(moveVector, upVec) * upVec;
        if(glm::length(moveVector) > 0.0f)
            moveVector = glm::normalize(moveVector) * this->speed;
    }

    if(Input::IsKeyDown(GLFW_KEY_SPACE))
        moveVector += this->speed * upVec;
    if(Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
        moveVector -= this->speed * upVec;

    if(glm::length(moveVector) > 0.0f)
        moveVector = glm::normalize(moveVector) * this->speed * GameEngine::instance->DeltaTime();

    transform->MovePosBy(moveVector);

    if(GameEngine::input->IsKeyPressed(GLFW_KEY_ESCAPE))
        Input::ToggleTrappedCursor();
}

void Component::Movement::FixedUpdate()
{
}

void Component::Movement::ImGuiUpdate()
{
    ImGui::Begin("Player controller");
    ImGui::DragFloat("Speed", &this->speed, 0.1f, 0.1f, 20.0f);
    ImGui::Checkbox("Orient towards gravity", &this->orientTowardsGravity);
    ImGui::End();
}
