#include "Engine.h"

#include <iostream>
#include <format>

#include "Debug/Logger.h"
#include "GLWrapper/BasicShaderProgram.h"

#include "Engine/LevelLoad.h"

#include "Generator/MeshGenerator.h"

GameEngine* GameEngine::instance = nullptr;
Renderer* GameEngine::renderer = nullptr;
Level* GameEngine::currentLevel = nullptr;
Input* GameEngine::input = nullptr;
Lighting* GameEngine::lighting = nullptr;
Physics* GameEngine::physics = nullptr;
MaterialLibrary* GameEngine::materialLibrary = nullptr;

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
    delete physics;
    delete materialLibrary;
    glfwTerminate();
}

void GameEngine::Run(const EngineConfig::Config& config)
{
    Debug::LogInfo("Starting game engine");
    srand(static_cast<unsigned int>(time(nullptr)));

    InitializeGLFW(config);

    lighting = (new Lighting())->MakeInstance();
    lighting->SetDirectionalLightSource(
        glm::vec3(-0.8f, -0.8f, 0.3f),
        glm::vec3(0.5f, 0.5f, 0.5f)
    );
    lighting->SetAmbientIntensity(0.02f);

    renderer = (new Renderer(config.windowWidth, config.windowHeight, config.antiAliasingMethod, config.gamma))->MakeInstance();
    GL::Shader::LogGLErrors("After Renderer Init");

    currentLevel = new Level();
    input = (new Input())->MakeInstance();

    lighting->InitializeShadowMapping();
    GL::Shader::LogGLErrors("After Shadow Mapping Init");

    Renderer::SetVSYNC(config.VSync);

    physics = (new Physics())->MakeInstance();
    physics->SetGlobalGravity(glm::vec3(0.0f, 0.0f, 0.0f));

    materialLibrary = new MaterialLibrary();

    LoadLevel();

    Input::SetCursorMode(Input::CursorMode::Trapped);

    this->lastFrameTime = glfwGetTime();

    GL::Shader::LogGLErrors("Before Main Loop");
    Debug::LogInfo("Starting main loop");
    while (!renderer->ShouldClose())
    {
        this->CalculateDeltaTime();
        
        input->Update();

        physics->Update(this->deltaTime);

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
    this->FPS = 1.0f / this->deltaTime;

    this->fpsHistory.push_back(this->FPS);
    if (this->fpsHistory.size() > FPS_HISTORY_SIZE)
        this->fpsHistory.pop_front();

    this->avgFPS = 0;
    for (float fps : this->fpsHistory) this->avgFPS += fps;
    this->avgFPS /= this->fpsHistory.size();
}

void GameEngine::InitializeGLFW(const EngineConfig::Config& config)
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
