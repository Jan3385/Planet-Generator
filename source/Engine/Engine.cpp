#include "Engine.h"

#include <iostream>

#include "Debug/Logger.h"

#include "GLWrapper/BasicShaderProgram.h"
#include "Component/Essential/RenderComponent.h"
#include "Component/Player/MovementComponent.h"
#include "Object/GameObject.h"

GameEngine* GameEngine::instance = nullptr;
Renderer* GameEngine::renderer = nullptr;
Level* GameEngine::currentLevel = nullptr;
Input* GameEngine::input = nullptr;

GameEngine::GameEngine()
{
    instance = this;
}

GameEngine::~GameEngine()
{
    delete renderer;
    delete currentLevel;
}

void GameEngine::Run()
{
    InitializeGLFW();

    renderer = new Renderer();
    currentLevel = new Level();
    input = new Input();

    Renderer::SetVSYNC(true);

    // temp ----
    GL::BasicShaderProgram shader("RenderVertex");

    std::vector<float> vertices = {
        -0.5f,  -0.5f, -0.5f,
        0.5f,  -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  -0.5f, 0.5f,
        0.5f,  -0.5f, 0.5f,
        0.5f,  0.5f, 0.5f,
        -0.5f,  0.5f, 0.5f
    };
    std::vector<unsigned int> indices = {
        0, 1, 3, 3, 1, 2,
        1, 5, 2, 2, 5, 6,
        5, 4, 6, 6, 4, 7,
        4, 0, 7, 7, 0, 3,
        3, 2, 7, 7, 2, 6,
        4, 5, 0, 0, 5, 1
    }; 
    Object::GameObject *obj = currentLevel->CreateGameObject();
    
    obj->GetMesh()->SetMeshData(vertices, indices);

    Component::RenderComponent *renderComp = obj->GetRenderComponent();
    renderComp->SetRenderShader(&shader);

    Object::BaseObject *camObj = currentLevel->CreateObject();
    camObj->AddComponent<Component::Transform>()->SetPos(glm::vec3(0.0f, 0.0f, 2.5f));
    camObj->AddComponent<Component::Camera>();
    camObj->AddComponent<Component::Movement>();
    // ---------

    Input::SetCursorMode(Input::CursorMode::Trapped);

    while (!renderer->ShouldClose())
    {
        //obj->GetTransform()->RotateBy(glm::vec3(0.0f, 0.5f, 0.1f));
        glfwPollEvents();

        currentLevel->Update();

        renderer->Update();

        input->EndFrame();
    }

    glfwTerminate();
}

void GameEngine::InitializeGLFW()
{
    if (!glfwInit())
    {
        Debug::LogFatal("Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
