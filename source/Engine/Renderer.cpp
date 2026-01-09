#include "Renderer.h"

#include "Debug/Logger.h"

void UpdateViewport(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Renderer::Renderer()
{
    this->window = glfwCreateWindow(800, 600, "Planet renderer", nullptr, nullptr);

    if(!this->window) {
        Debug::LogFatal("Failed to create GLFW window");
        return;
    }

    glfwMakeContextCurrent(this->window);
    glfwSetFramebufferSizeCallback(this->window, UpdateViewport);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        Debug::LogFatal("Failed to initialize GLAD");
        return;
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

Renderer::~Renderer()
{
    if(this->window != nullptr) {
        glfwDestroyWindow(this->window);
        this->window = nullptr;
    }
}

void Renderer::Update()
{
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for(auto& callback : renderCallbacks) {
        callback->Render();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}
