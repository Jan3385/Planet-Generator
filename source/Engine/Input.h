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
    static void ToggleTrappedCursor();
    static CursorMode GetCursorMode() { return Input::currentCursorMode; }

    static bool IsKeyPressed(int keycode);
    static bool IsKeyDown(int keycode);
    static bool IsKeyUp(int keycode);
    static bool IsKeyReleased(int keycode);

    static glm::vec2 GetMovementVector();
    
    static bool IsMouseButtonDown(MouseButton button);
    static bool IsMouseButtonUp(MouseButton button);

    glm::vec2 GetCursor() const { return cursorPos; }
    glm::vec2 GetCursorDelta() const;

    glm::vec2 GetScrollDelta() const { return scrollDelta; }

    void Update();
    void EndFrame();
private:
    static bool ignoreKeyboardInput;
    static bool ignoreMouseInput;

    bool firstCursorUpdate = true;

    static CursorMode currentCursorMode;
    glm::vec2 cursorPos;
    glm::vec2 cursorDelta = glm::vec2(0.0f, 0.0f);

    static bool currKey[GLFW_KEY_LAST - GLFW_KEY_SPACE + 1];
    static bool prevKey[GLFW_KEY_LAST - GLFW_KEY_SPACE + 1];

    glm::vec2 scrollDelta = glm::vec2(0.0f, 0.0f);
    void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};