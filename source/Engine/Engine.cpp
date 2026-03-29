#include "Engine.h"

#include <iostream>
#include <format>

#include "Debug/Logger.h"
#include "GLWrapper/BasicShaderProgram.h"
#include "GLWrapper/Model.h"
#include "GLWrapper/Cubemap.h"
#include "Component/Game/Player/MovementComponent.h"
#include "Component/Game/Planet/PlanetGenComponent.h"
#include "Component/Engine/PointLightSourceComponent.h"
#include "Object/GameObject.h"

#include "Generator/MeshGenerator.h"

GameEngine* GameEngine::instance = nullptr;
Renderer* GameEngine::renderer = nullptr;
Level* GameEngine::currentLevel = nullptr;
Input* GameEngine::input = nullptr;
Lighting* GameEngine::lighting = nullptr;
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
    delete materialLibrary;
    glfwTerminate();
}

void GameEngine::Run(const EngineConfig::Config& config)
{
    Debug::LogInfo("Starting game engine");
    srand(static_cast<unsigned int>(time(nullptr)));

    InitializeGLFW(config);

    lighting = new Lighting();
    lighting->SetDirectionalLightSource(
        glm::vec3(-0.8f, -0.8f, 0.3f),
        glm::vec3(0.5f, 0.5f, 0.5f)
    );
    lighting->SetAmbientIntensity(0.02f);

    renderer = new Renderer(config.windowWidth, config.windowHeight, config.antiAliasingMethod, config.gamma);
    GL::Shader::LogGLErrors("After Renderer Init");

    currentLevel = new Level();
    input = new Input();

    lighting->InitializeShadowMapping();
    GL::Shader::LogGLErrors("After Shadow Mapping Init");

    Renderer::SetVSYNC(config.VSync);

    materialLibrary = new MaterialLibrary();
    materialLibrary->CreateMaterial("red", &renderer->GetDefaultColorShader())
        ->SetValue("objectColor", glm::vec3(1.0f, 0.0f, 0.0f))
        ->SetTransparency(Object::Material::Transparency::OpaqueNoLight)
        ->attributes = Object::Material::RenderAttributes::Default;

    materialLibrary->CreateMaterial("white", &renderer->GetDefaultColorShader())
        ->SetValue("objectColor", glm::vec3(1.0f, 1.0f, 1.0f))
        ->SetTransparency(Object::Material::Transparency::OpaqueNoLight)
        ->attributes = Object::Material::RenderAttributes::Default;

    std::vector<std::string> cubemapPaths{
        "Images/Skybox/skybox-right.jpg",
        "Images/Skybox/skybox-left.jpg",
        "Images/Skybox/skybox-bottom.jpg",
        "Images/Skybox/skybox-top.jpg",
        "Images/Skybox/skybox-front.jpg",
        "Images/Skybox/skybox-back.jpg"
    };
    GL::Cubemap skyboxCubemap(cubemapPaths.data(), false, true);
    materialLibrary->CreateMaterial("skybox", &renderer->GetSkyboxShader())
        ->SetTransparency(Object::Material::Transparency::Special)
        ->SetTexture(&skyboxCubemap)
        ->attributes = Object::Material::RenderAttributes::ReverseFaceCulling | 
            Object::Material::RenderAttributes::ViewNoTransformMatrix | Object::Material::RenderAttributes::Depth_LEQUAL;  

    // temp ----
    GL::BasicShaderProgram planetShader("PlanetShader");

    GL::BasicShaderProgram atmosphereShader("AtmosphereShader");
    lighting->RegisterShaderLightUpdateCallback(&atmosphereShader);
    GL::Shader::LogGLErrors("After Engine Shaders");

    std::shared_ptr<GL::Mesh> cube;
    cube = MeshGenerator::GenerateCubeMesh();
    
    std::shared_ptr<GL::Mesh> spherifiedCube;
    spherifiedCube = MeshGenerator::GenerateSpherifiedCubeMesh(60);

    // Skybox
    Object::BaseObject *skyboxObj = currentLevel->CreateObject();
    Component::MeshRender* skyboxMeshRenderer = skyboxObj->AddComponent<Component::MeshRender>()
        ->SetMaterial(materialLibrary->GetMaterial("skybox"))
        ->SetMesh(cube);
    currentLevel->SetSkybox(skyboxMeshRenderer);
    GL::Shader::LogGLErrors("After Skybox Creation");

    // Planet obj
    constexpr float planetScale = 2.0f;
    Material *planetMat = materialLibrary->CreateMaterial("planet", &planetShader);
    Material *atmosphereMat = materialLibrary->CreateMaterial("atmosphere", &atmosphereShader);

    planetMat
        ->SetTransparency(Object::Material::Transparency::Opaque)
        ->attributes = Object::Material::RenderAttributes::Default;
    atmosphereMat
        ->SetTransparency(Object::Material::Transparency::Transparent)
        ->attributes = 
            Object::Material::RenderAttributes::Transform | Object::Material::RenderAttributes::NormalMatrix | 
            Object::Material::RenderAttributes::ReverseFaceCulling | Object::Material::RenderAttributes::OriginPos;


    Object::BaseObject *planet = currentLevel->CreateObject();
    planet->AddComponent<Component::Transform>()->SetScale(glm::vec3(planetScale));

    Component::PlanetGen *planetGenComp = planet->AddComponent<Component::PlanetGen>();
    planetGenComp->SetMaterials("planet", "atmosphere");
    planetGenComp->SetAtmosphereColors(
        glm::vec4(0.2f, 0.5f, 1.0f, 1.0f), 
        glm::vec4(0.0f, 0.0f, 0.5f, 1.0f)
    );
    planetGenComp->SetPlanetColors(
        { glm::vec3(0.0f, 0.3f, 1.0f),    glm::vec2(0.1f, 0.5f)},
        { glm::vec3(0.04f, 0.55f, 1.0f),  glm::vec2(0.1f, 0.375f)},
        { glm::vec3(0.76f, 0.70f, 0.50f), glm::vec2(0.0f, 0.0625f)},
        { glm::vec3(0.1f, 0.55f, 0.1f),   glm::vec2(0.0f, 0.03125f)},
        { glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec2(0.15f, 0.04f)},
        { glm::vec3(1.0f, 1.0f, 1.0f),    glm::vec2(0.0f, 0.02f)}
    );

    planetGenComp->PlanetifyMesh(rand());
    // -----

    // model import
    Object::GameObject *modelObj = currentLevel->CreateGameObject();
    modelObj->GetTransform()
        ->SetPos(glm::vec3(1.7f, 0.0f, 1.7f))
        ->SetScale(glm::vec3(0.25f))
        ->SetRot(glm::vec3(0.0f, -45.0f, 0.0f));

    Component::MeshRender *modelRenderComp = modelObj->GetRenderComponent();
    std::shared_ptr<GL::Model> modelMesh = std::make_shared<GL::Model>("Models/teapot.obj");
    modelRenderComp->SetMesh(modelMesh);

    modelObj->SetParent(planet);
    GL::Shader::LogGLErrors("After Model Loading");

    // floor
    Object::GameObject *floor = currentLevel->CreateGameObject();
    floor->GetTransform()
        ->SetScale(glm::vec3(80.0f, 0.1f, 80.0f))
        ->SetPos(glm::vec3(0.0f, -2.0f, 0.0f));
    Component::MeshRender *floorRenderComp = floor->GetRenderComponent();
    floorRenderComp->SetMesh(cube);

    // lights
    Object::BaseObject *lightObj = currentLevel->CreateLightObject(Math::RGB(255, 0, 0), 3.5f);
    lightObj->GetComponent<Component::Transform>()->SetPos(glm::vec3(1.0f, -1.2f, 2.5f));
    lightObj->GetComponent<Component::MeshRender>()
        ->SetMesh(cube)
        ->SetMaterial(materialLibrary->GetMaterial("red"));

    Object::BaseObject *lightObj2 = currentLevel->CreateLightObject(Math::RGB(255, 255, 255), 2.5f);
    lightObj2->GetComponent<Component::Transform>()->SetPos(glm::vec3(0.0f, -1.2f, 2.5f));
    lightObj2->GetComponent<Component::MeshRender>()
        ->SetMesh(cube)
        ->SetMaterial(materialLibrary->GetMaterial("white"));

    Object::GameObject *shadowCaster = currentLevel->CreateGameObject();
    shadowCaster->GetTransform()
        ->SetPos(glm::vec3(0.5f, -1.8f, 2.8f))
        ->SetScale(glm::vec3(0.3f));
    shadowCaster->GetRenderComponent()->SetMesh(cube);

    //lightObj->Disable();
    //lightObj2->Disable();
    //shadowCaster->Disable();
    //floor->Disable();
    //modelObj->Disable();
    GL::Shader::LogGLErrors("After Creating Lights");

    // Camera obj
    Object::BaseObject *camObj = currentLevel->CreateObject();
    camObj->AddComponent<Component::Transform>()->SetPos(glm::vec3(0.0f, 0.5f, 2.5f));
    camObj->AddComponent<Component::Camera>();
    camObj->AddComponent<Component::Movement>();

    // makes the player a point light source
    //Component::PointLightSource *pointLight = camObj->AddComponent<Component::PointLightSource>();
    //Component::PointLightSource::PointLightData lightData;
    //lightData.color = glm::vec3(1.0f, 1.0f, 1.0f);
    //pointLight->SetLightData(lightData);

    GL::Shader::LogGLErrors("After Object Creation");
    // ---------

    Input::SetCursorMode(Input::CursorMode::Trapped);

    this->lastFrameTime = glfwGetTime();

    GL::Shader::LogGLErrors("Before Main Loop");
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
