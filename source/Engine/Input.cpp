#include "Input.h"

#include "Engine/Engine.h"

constexpr int KEY_OFFSET = GLFW_KEY_SPACE;
Input::CursorMode Input::currentCursorMode = Input::CursorMode::Normal;
bool Input::currKey[GLFW_KEY_LAST - GLFW_KEY_SPACE + 1] = { false };
bool Input::prevKey[GLFW_KEY_LAST - GLFW_KEY_SPACE + 1] = { false };

Input::Input()
{
    glfwSetCursorPosCallback(GameEngine::renderer->window,
        [](GLFWwindow* window, double xpos, double ypos) {
            GameEngine::input->CursorPositionCallback(window, xpos, ypos);
        });
    
    glfwSetScrollCallback(GameEngine::renderer->window,
        [](GLFWwindow* window, double xoffset, double yoffset) {
            GameEngine::input->MouseScrollCallback(window, xoffset, yoffset);
        });
}

Input::~Input()
{

}

void Input::SetCursorMode(CursorMode mode)
{
    Input::currentCursorMode = mode;
    glfwSetInputMode(GameEngine::renderer->window, GLFW_CURSOR, static_cast<int>(mode));
}

void Input::ToggleTrappedCursor()
{
    if(Input::currentCursorMode == CursorMode::Trapped) SetCursorMode(CursorMode::Normal);
    else SetCursorMode(CursorMode::Trapped);
}

bool Input::IsKeyPressed(int keycode)
{
    return Input::currKey[keycode - KEY_OFFSET] && !Input::prevKey[keycode - KEY_OFFSET];
}

bool Input::IsKeyDown(int keycode)
{
    return Input::currKey[keycode - KEY_OFFSET];
}

bool Input::IsKeyUp(int keycode)
{
    return !Input::currKey[keycode - KEY_OFFSET];
}

bool Input::IsKeyReleased(int keycode)
{
    return !Input::currKey[keycode - KEY_OFFSET] && Input::prevKey[keycode - KEY_OFFSET];
}

glm::vec2 Input::GetMovementVector()
{
    glm::vec2 movement(0.0f, 0.0f);
    if(Input::IsKeyDown(GLFW_KEY_W))
        movement.y += 1.0f;
    if(Input::IsKeyDown(GLFW_KEY_S))
        movement.y -= 1.0f;
    if(Input::IsKeyDown(GLFW_KEY_A))
        movement.x -= 1.0f;
    if(Input::IsKeyDown(GLFW_KEY_D))
        movement.x += 1.0f;
    return movement;
}

bool Input::IsMouseButtonDown(MouseButton button)
{
    return glfwGetMouseButton(GameEngine::renderer->window, static_cast<int>(button)) == GLFW_PRESS;
}

bool Input::IsMouseButtonUp(MouseButton button)
{
    return glfwGetMouseButton(GameEngine::renderer->window, static_cast<int>(button)) == GLFW_RELEASE;
}

glm::vec2 Input::GetCursorDelta() const
{
    return cursorDelta;
}

void Input::Update()
{
    glfwPollEvents();

    for(int key = KEY_OFFSET; key <= GLFW_KEY_LAST; key++){
        Input::currKey[key - KEY_OFFSET] = glfwGetKey(GameEngine::renderer->window, key) == GLFW_PRESS;
    }
}

void Input::EndFrame()
{
    cursorDelta = glm::vec2(0.0f, 0.0f);
    scrollDelta = glm::vec2(0.0f, 0.0f);

    std::copy(std::begin(Input::currKey), std::end(Input::currKey), std::begin(Input::prevKey));
}

void Input::CursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
    if(firstCursorUpdate){
        cursorPos = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
        firstCursorUpdate = false;
        return;
    }

    glm::vec2 newPos = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
    cursorDelta = newPos - cursorPos;
    cursorPos = newPos;
}

void Input::MouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    scrollDelta = glm::vec2(static_cast<float>(xoffset), static_cast<float>(yoffset));   
}
