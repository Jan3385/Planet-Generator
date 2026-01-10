#include "Renderer.h"

#include "Debug/Logger.h"
#include "Engine/Engine.h"

void UpdateViewport(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    GameEngine::renderer->StoreWindowSize(width, height);
}

void Renderer::StoreWindowSize(int width, int height)
{
    this->windowWidth = width;
    this->windowHeight = height;
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

    glEnable(GL_DEPTH_TEST);
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

Renderer::~Renderer()
{
    if(this->window != nullptr) {
        glfwDestroyWindow(this->window);
        this->window = nullptr;
    }
}

void Renderer::SetVSYNC(bool enabled)
{
    if(!enabled) glfwSwapInterval(0);
    else glfwSwapInterval(1);
}

void Renderer::Update()
{
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Component::Camera* camera = GameEngine::currentLevel->GetCamera();
    camera->SetAspectRatio(static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight));

    glm::mat4 projection = camera->GetProjection();
    glm::mat4 view = camera->GetView();

    for(auto& callback : renderCallbacks) {
        callback->Render(projection, view);
    }
    
    glfwSwapBuffers(window);
}
