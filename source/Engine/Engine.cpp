#include "Engine.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Debug/Logger.h"

#include "GLWrapper/BasicShaderProgram.h"
#include "Component/Essential/RenderComponent.h"

GameEngine* GameEngine::instance = nullptr;

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
    if (!glfwInit())
    {
        Debug::LogFatal("Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    renderer = new Renderer();
    currentLevel = new Level();

    // temp ----
    GL::BasicShaderProgram shader("RenderVertex");

    std::vector<float> vertices = {
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    std::vector<unsigned int> indices = {
        0, 1, 3,
        1, 2, 3
    }; 
    Object::BaseObject *obj = currentLevel->CreateObject();
    
    Component::Mesh *mesh = obj->AddComponent<Component::Mesh>();
    mesh->SetMeshData(vertices, indices);

    Component::RenderComponent *renderComp = obj->AddComponent<Component::RenderComponent>();
    renderComp->SetMeshComponent(mesh);
    renderComp->SetRenderShader(&shader);

    // ---------

    while (!renderer->ShouldClose())
    {
        renderer->Update();
    }

    glfwTerminate();
}
