#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

class Renderer{
public:
    struct IRendererCallback{
    protected:
        virtual void Render(glm::mat4 &projection, glm::mat4 &view) = 0;
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

    void StoreWindowSize(int width, int height);
private:
    GLFWwindow* window = nullptr;
    std::vector<IRendererCallback*> renderCallbacks;

    int windowWidth = 800;
    int windowHeight = 600;
};