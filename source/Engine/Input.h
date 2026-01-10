#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Input{
public:
    Input();
    ~Input();

    enum class MouseButton{
        Left = GLFW_MOUSE_BUTTON_LEFT,
        Right = GLFW_MOUSE_BUTTON_RIGHT,
        Middle = GLFW_MOUSE_BUTTON_MIDDLE
    };

    enum class CursorMode{
        Normal = GLFW_CURSOR_NORMAL,
        Hidden = GLFW_CURSOR_HIDDEN,
        Trapped = GLFW_CURSOR_DISABLED
    };

    static void SetCursorMode(CursorMode mode);

    static bool IsKeyDown(int keycode);
    static bool IsKeyUp(int keycode);

    static glm::vec2 GetMovementVector();
    
    static bool IsMouseButtonDown(MouseButton button);
    static bool IsMouseButtonUp(MouseButton button);

    glm::vec2 GetCursor() const { return cursorPos; }
    glm::vec2 GetCursorDelta() const;

    glm::vec2 GetScrollDelta() const { return scrollDelta; }

    void EndFrame();
private:
    bool firstCursorUpdate = true;
    glm::vec2 cursorPos;
    glm::vec2 cursorDelta = glm::vec2(0.0f, 0.0f);

    glm::vec2 scrollDelta = glm::vec2(0.0f, 0.0f);
    void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};