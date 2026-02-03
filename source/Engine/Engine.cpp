#include "Engine.h"

#include <iostream>

#include "Debug/Logger.h"
#include "GLWrapper/BasicShaderProgram.h"
#include "Component/Essential/Renderer/PhongMeshRenderComponent.h"
#include "Component/Essential/Renderer/ColorMeshRenderComponent.h"
#include "Component/Essential/Renderer/PlanetMeshRenderComponent.h"
#include "Component/Essential/Renderer/AtmosphereRenderComponent.h"
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
    delete currentLevel;
    delete input;
    delete lighting;
    delete renderer;

    glfwTerminate();
}

void GameEngine::Run(const Config& config)
{
    srand(static_cast<unsigned int>(time(nullptr)));

    InitializeGLFW(config);

    lighting = new Lighting();
    lighting->SetDirectionalLightSource(
        glm::vec3(-0.8f, 0.3f, 0.3f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f)
    );

    renderer = new Renderer(config.windowWidth, config.windowHeight, config.MSAA_Samples);
    currentLevel = new Level();
    input = new Input();

    Renderer::SetVSYNC(config.VSync);

    // temp ----
    GL::BasicShaderProgram planetShader("PlanetShader");
    lighting->RegisterShaderLightUpdateCallback(&planetShader);
    GL::BasicShaderProgram atmosphereShader("AtmosphereShader");
    lighting->RegisterShaderLightUpdateCallback(&atmosphereShader);
    GL::BasicShaderProgram atmosphereOutsideShader("AtmosphereShaderOutside");
    lighting->RegisterShaderLightUpdateCallback(&atmosphereOutsideShader);

    std::shared_ptr<GL::Mesh> cube;
    cube = MeshGenerator::GenerateCubeMesh();
    
    std::shared_ptr<GL::Mesh> spherifiedCube;
    spherifiedCube = MeshGenerator::GenerateSpherifiedCubeMesh(60);

    // Normal obj
    Object::BaseObject *planet = currentLevel->CreateObject();
    constexpr float planetScale = 1.0f;
    planet->AddComponent<Component::Transform>()->SetScale(glm::vec3(planetScale));

    Component::PlanetMeshRender *renderComp = planet->AddComponent<Component::PlanetMeshRender>();
    renderComp->SetRenderShader(&planetShader);
    renderComp->SetMesh(spherifiedCube);

    using Component::PlanetMeshRender;

    PlanetMeshRender::planetPalette palette{
        PlanetMeshRender::MaterialSTD140(
            glm::vec3(0.0f, 0.3f, 1.0f) * 0.5f,
            glm::vec3(0.0f, 0.3f, 1.0f),
            glm::vec3(0.6f, 0.6f, 0.6f),
            0.5f),
        PlanetMeshRender::MaterialSTD140(
            glm::vec3(0.04f, 0.55f, 1.0f) * 0.5f,
            glm::vec3(0.04f, 0.55f, 1.0f),
            glm::vec3(0.5f, 0.5f, 0.5f),
            0.375f),
        PlanetMeshRender::MaterialSTD140(
            glm::vec3(0.76f, 0.70f, 0.50f) * 0.5f,
            glm::vec3(0.76f, 0.70f, 0.50f),
            glm::vec3(0.15f, 0.15f, 0.15f),
            0.0625f),
        PlanetMeshRender::MaterialSTD140(
            glm::vec3(0.1f, 0.6f, 0.1f) * 0.5f,
            glm::vec3(0.1f, 0.6f, 0.1f),
            glm::vec3(0.05f, 0.05f, 0.05f),
            0.03125f),
        PlanetMeshRender::MaterialSTD140(
            glm::vec3(0.5f, 0.5f, 0.5f) * 0.5f,
            glm::vec3(0.5f, 0.5f, 0.5f),
            glm::vec3(0.2f, 0.2f, 0.2f),
            0.125f),
        PlanetMeshRender::MaterialSTD140(
            glm::vec3(1.0f, 1.0f, 1.0f) * 0.5f,
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(0.4f, 0.4f, 0.4f),
            0.25f)
    };

    renderComp->SetColorPalette(palette);

    Component::AtmosphereRender *atmosphereRenderComp = planet->AddComponent<Component::AtmosphereRender>();
    atmosphereRenderComp->SetRenderShader(&atmosphereOutsideShader);
    atmosphereRenderComp->SetRenderShaderInside(&atmosphereShader);
    atmosphereRenderComp->SetColorPalette({ glm::vec4(0.2f, 0.5f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.5f, 1.0f) });

    planet->AddComponent<Component::PlanetGen>()->PlanetifyMesh(rand());

    // floor
    Object::GameObject *floor = currentLevel->CreateGameObject();
    floor->GetTransform()
        ->SetScale(glm::vec3(8.0f, 0.1f, 8.0f))
        ->SetPos(glm::vec3(0.0f, -2.0f, 0.0f));
    Component::PhongMeshRender *floorRenderComp = floor->GetRenderComponent();
    floorRenderComp->SetMaterial(GetMaterial(MatIndex::WhitePlastic));
    floorRenderComp->SetMesh(cube);

    floor->Disable();


    Object::BaseObject *lightObj = currentLevel->CreateLightObject(Math::RGB(255, 0, 0));
    lightObj->GetComponent<Component::Transform>()->SetPos(glm::vec3(0.8f, 0.8f, 0.8f));
    lightObj->GetComponent<Component::ColorMeshRender>()->SetMesh(cube);
    lightObj->Disable();

    Object::BaseObject *lightObj2 = currentLevel->CreateLightObject(Math::RGB(255, 255, 255));
    lightObj2->GetComponent<Component::Transform>()->SetPos(glm::vec3(0.0f, -0.5f, 1.5f));
    lightObj2->GetComponent<Component::ColorMeshRender>()->SetMesh(cube);
    lightObj2->Disable();

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
}

void GameEngine::CalculateDeltaTime()
{
    double currentFrameTime = glfwGetTime();
    this->deltaTime = static_cast<float>(currentFrameTime - this->lastFrameTime);
    this->lastFrameTime = currentFrameTime;
}

void GameEngine::InitializeGLFW(const Config& config)
{
    glfwSetErrorCallback([](int error, const char* description) {
        Debug::LogError(std::format("GLFW Error ({0}): {1}", error, description));
    });

    if(glfwPlatformSupported(GLFW_PLATFORM_WIN32)) 
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WIN32);
    else if(glfwPlatformSupported(GLFW_PLATFORM_WAYLAND)) 
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    else if(glfwPlatformSupported(GLFW_PLATFORM_X11)) 
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    else glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_NULL);


    if (!glfwInit())
    {
        Debug::LogFatal("Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
