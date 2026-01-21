#include "Engine.h"

#include <iostream>

#include "Debug/Logger.h"
#include "GLWrapper/BasicShaderProgram.h"
#include "Component/Essential/Renderer/PhongMeshRenderComponent.h"
#include "Component/Essential/Renderer/ColorMeshRenderComponent.h"
#include "Component/Essential/Renderer/PlanetMeshRenderComponent.h"
#include "Component/Player/MovementComponent.h"
#include "Component/Planet/PlanetGenComponent.h"
#include "Component/Essential/PointLightSourceComponent.h"
#include "Object/GameObject.h"

#include "Generator/MeshGenerator.h"

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
    Debug::Logger::Instance().minLogLevel = Debug::Logger::Level::SPAM;

    InitializeGLFW();

    lighting = new Lighting();
    renderer = new Renderer();
    currentLevel = new Level();
    input = new Input();
    lighting->SetDirectionalLightSource(
        glm::vec3(-0.2f, 1.0f, -0.3f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f)
    );

    Renderer::SetVSYNC(true);

    // temp ----
    GL::BasicShaderProgram planetShader("PlanetShader");
    lighting->RegisterShaderLightUpdateCallback(&planetShader);

    GL::Mesh cube;
    cube = MeshGenerator::GenerateCubeMesh();
    
    GL::Mesh spherifiedCube;
    spherifiedCube = MeshGenerator::GenerateSpherifiedCubeMesh(50);
    GL::ColorMesh planetMesh(spherifiedCube);

    // Normal obj
    Object::BaseObject *planet = currentLevel->CreateObject();
    constexpr float planetScale = 1.0f;
    planet->AddComponent<Component::Transform>()->SetScale(glm::vec3(planetScale));

    Component::PlanetMeshRender *renderComp = planet->AddComponent<Component::PlanetMeshRender>();
    renderComp->SetRenderShader(&planetShader);
    renderComp->SetMesh(&planetMesh);
    //renderComp->SetMaterial(GetMaterial(MatIndex::Ruby));

    planet->AddComponent<Component::PlanetGen>()->PlanetifyMesh(42);

    // floor
    Object::GameObject *floor = currentLevel->CreateGameObject();
    floor->GetTransform()
        ->SetScale(glm::vec3(8.0f, 0.1f, 8.0f))
        ->SetPos(glm::vec3(0.0f, -2.0f, 0.0f));
    Component::PhongMeshRender *floorRenderComp = floor->GetRenderComponent();
    floorRenderComp->SetMaterial(GetMaterial(MatIndex::WhitePlastic));
    floorRenderComp->SetMesh(&cube);

    floor->Disable();


    Object::BaseObject *lightObj = currentLevel->CreateLightObject(Math::RGB(255, 0, 0));
    lightObj->GetComponent<Component::Transform>()->SetPos(glm::vec3(0.8f, 0.8f, 0.8f));
    lightObj->GetComponent<Component::ColorMeshRender>()->SetMesh(&cube);

    Object::BaseObject *lightObj2 = currentLevel->CreateLightObject(Math::RGB(255, 255, 255));
    lightObj2->GetComponent<Component::Transform>()->SetPos(glm::vec3(0.0f, -0.5f, 1.5f));
    lightObj2->GetComponent<Component::ColorMeshRender>()->SetMesh(&cube);

    Object::BaseObject *lightObj3 = currentLevel->CreateLightObject(Math::RGB(51, 255, 51));
    lightObj3->GetComponent<Component::Transform>()->SetPos(glm::vec3(0.4f, 0.8f, -0.8f));
    lightObj3->GetComponent<Component::ColorMeshRender>()->SetMesh(&cube);
    
    // Camera obj
    Object::BaseObject *camObj = currentLevel->CreateObject();
    camObj->AddComponent<Component::Transform>()->SetPos(glm::vec3(0.0f, 0.0f, 2.5f));
    camObj->AddComponent<Component::Camera>();
    camObj->AddComponent<Component::Movement>();
    // ---------

    Input::SetCursorMode(Input::CursorMode::Trapped);

    this->lastFrameTime = glfwGetTime();

    Debug::LogInfo("Starting main loop");
    while (!renderer->ShouldClose())
    {
        this->CalculateDeltaTime();

        input->Update();

        currentLevel->Update(); 

        renderer->Update();

        input->EndFrame();
    }

    glfwTerminate();
}

void GameEngine::CalculateDeltaTime()
{
    double currentFrameTime = glfwGetTime();
    this->deltaTime = static_cast<float>(currentFrameTime - this->lastFrameTime);
    this->lastFrameTime = currentFrameTime;
}

void GameEngine::InitializeGLFW()
{
    if (glfwPlatformSupported(GLFW_PLATFORM_WAYLAND)){
      glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    }
    else if (glfwPlatformSupported(GLFW_PLATFORM_X11)){
      glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    }
    else if (glfwPlatformSupported(GLFW_PLATFORM_WIN32)){
      glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WIN32);
    }
    else { // fallback
      glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_NULL);
    }

    if (!glfwInit())
    {
        Debug::LogFatal("Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
