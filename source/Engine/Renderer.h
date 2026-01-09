#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

class Renderer{
public:
    struct IRendererCallback{
    protected:
        virtual void Render() = 0;
        virtual ~IRendererCallback() = default;

        friend class Renderer;
    };

    Renderer();
    ~Renderer();

    void AddRenderCallback(IRendererCallback* callback) {
        renderCallbacks.push_back(callback);
    }
    void RemoveRenderCallback(IRendererCallback* callback) {
        std::erase(renderCallbacks, callback);
    }

    bool ShouldClose() const { return glfwWindowShouldClose(this->window); }
    void Update();
private:
    GLFWwindow* window = nullptr;
    std::vector<IRendererCallback*> renderCallbacks;
};