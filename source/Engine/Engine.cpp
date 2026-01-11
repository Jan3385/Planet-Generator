#include "Engine.h"

#include <iostream>

#include "Debug/Logger.h"

#include "GLWrapper/BasicShaderProgram.h"
#include "Component/Essential/RenderComponent.h"
#include "Component/Player/MovementComponent.h"
#include "Component/Essential/PointLightSourceComponent.h"
#include "Object/GameObject.h"

GameEngine* GameEngine::instance = nullptr;
Renderer* GameEngine::renderer = nullptr;
Level* GameEngine::currentLevel = nullptr;
Input* GameEngine::input = nullptr;
Lighting* GameEngine::lighting = nullptr;

GameEngine::GameEngine()
{
    instance = this;
}

GameEngine::~GameEngine()
{
    delete renderer;
    delete currentLevel;
    delete input;
    delete lighting;
}

void GameEngine::Run()
{
    InitializeGLFW();

    renderer = new Renderer();
    currentLevel = new Level();
    input = new Input();
    lighting = new Lighting();

    Renderer::SetVSYNC(true);

    // temp ----
    GL::BasicShaderProgram lightShader("LightedShader");
    lightShader.Use();
    lightShader.SetVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.2f));
    lighting->RegisterShaderLightUpdateCallback(&lightShader);

    GL::BasicShaderProgram colorShader("BasicShader.vert", "ColorShader.frag", "Color Shader");
    colorShader.Use();
    colorShader.SetVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));

    std::vector<float> vertices = {
        -0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,  
        0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f,  
        0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f,  0.5f, -0.5f, 
        0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
    };
    std::vector<float> normals = {
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f, 
        0.0f,  0.0f, -1.0f, 
        0.0f,  0.0f, -1.0f, 
        0.0f,  0.0f, -1.0f, 
        0.0f,  0.0f, -1.0f, 
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f
    };

    // Normal obj
    Object::GameObject *obj = currentLevel->CreateGameObject();
    
    obj->GetMesh()->SetMeshData(vertices, normals);

    Component::RenderComponent *renderComp = obj->GetRenderComponent();
    renderComp->SetRenderShader(&lightShader);

    // Light source obj
    Object::GameObject *lightObj = currentLevel->CreateGameObject();

    lightObj->GetMesh()->SetMeshData(vertices, normals);
    Component::Transform *lightTransform = lightObj->GetTransform();

    lightTransform->SetScale(glm::vec3(0.2f));
    lightTransform->SetPos(glm::vec3(0.8f, 0.8f, 0.8f));

    lightObj->GetRenderComponent()->SetRenderShader(&colorShader);
    lightObj->GetRenderComponent()->passLightDataToShader = false;

    Component::PointLightSource *pointLight = lightObj->AddComponent<Component::PointLightSource>();
    pointLight->SetLightData(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, 5.0f);



    Object::GameObject *lightObj2 = currentLevel->CreateGameObject();

    lightObj2->GetMesh()->SetMeshData(vertices, normals);
    Component::Transform *lightTransform2 = lightObj2->GetTransform();

    lightTransform2->SetScale(glm::vec3(0.2f));
    lightTransform2->SetPos(glm::vec3(0.0f, -0.5f, 1.5f));

    lightObj2->GetRenderComponent()->SetRenderShader(&colorShader);
    lightObj2->GetRenderComponent()->passLightDataToShader = false;

    Component::PointLightSource *pointLight2 = lightObj2->AddComponent<Component::PointLightSource>();
    pointLight2->SetLightData(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, 5.0f);

    // Camera obj
    Object::BaseObject *camObj = currentLevel->CreateObject();
    camObj->AddComponent<Component::Transform>()->SetPos(glm::vec3(0.0f, 0.0f, 2.5f));
    camObj->AddComponent<Component::Camera>();
    camObj->AddComponent<Component::Movement>();
    // ---------

    Input::SetCursorMode(Input::CursorMode::Trapped);

    while (!renderer->ShouldClose())
    {
        obj->GetTransform()->RotateBy(glm::vec3(0.1f, 0.3f, 0.04f));
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
