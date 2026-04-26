#pragma once

#include <glm/glm.hpp>
#include "Object/Material/MaterialLibrary.h"
#include "Generator/MeshGenerator.h"
#include "GLWrapper/Cubemap.h"
#include "GLWrapper/Model.h"
#include "Component/Game/Planet/PlanetGenComponent.h"
#include "Component/Game/Planet/GravityGeneratorComponent.h"
#include "Component/Game/Planet/GravityForceComponent.h"
#include "Component/Game/Player/MovementComponent.h"
#include "Component/Engine/Physics/SphereColliderComponent.h"

void LoadLevel(){
    MaterialLibrary* materialLibrary = GameEngine::materialLibrary;
    materialLibrary->CreateMaterial("red", &GameEngine::renderer->GetDefaultColorShader())
        ->SetValue("objectColor", glm::vec3(1.0f, 0.0f, 0.0f))
        ->SetTransparency(Object::Material::Transparency::OpaqueNoLight)
        ->attributes = Object::Material::RenderAttributes::Transform;

    materialLibrary->CreateMaterial("white", &GameEngine::renderer->GetDefaultColorShader())
        ->SetValue("objectColor", glm::vec3(1.0f, 1.0f, 1.0f))
        ->SetTransparency(Object::Material::Transparency::OpaqueNoLight)
        ->attributes = Object::Material::RenderAttributes::Transform;

    std::vector<std::string> cubemapPaths{
        "Images/Skybox/skybox-right.jpg",
        "Images/Skybox/skybox-left.jpg",
        "Images/Skybox/skybox-bottom.jpg",
        "Images/Skybox/skybox-top.jpg",
        "Images/Skybox/skybox-front.jpg",
        "Images/Skybox/skybox-back.jpg"
    };
    GL::Cubemap skyboxCubemap = GL::Cubemap(cubemapPaths.data(), false, true);
    GL::Cubemap &storedSkyboxCubemap = Renderer::Ins()->StoreCubemap("skybox", skyboxCubemap);

    materialLibrary->CreateMaterial("skybox", &GameEngine::renderer->GetSkyboxShader())
        ->SetTransparency(Object::Material::Transparency::Special)
        ->SetTexture(&storedSkyboxCubemap)
        ->attributes = Object::Material::RenderAttributes::ReverseFaceCulling | 
            Object::Material::RenderAttributes::ViewNoTransformMatrix | Object::Material::RenderAttributes::Depth_LEQUAL;  

    GL::BasicShaderProgram *planetShader = &Renderer::Ins()->RegisterShader("PlanetShader");

    GL::BasicShaderProgram *atmosphereShader = &Renderer::Ins()->RegisterShader("AtmosphereShader");
    GameEngine::lighting->RegisterShaderLightUpdateCallback(atmosphereShader);
    GL::Shader::LogGLErrors("After Engine Shaders");

    std::shared_ptr<GL::Mesh> cube;
    cube = MeshGenerator::GenerateCubeMesh();
    
    std::shared_ptr<GL::Mesh> spherifiedCube;
    spherifiedCube = MeshGenerator::GenerateSpherifiedCubeMesh(60);

    // Skybox
    Object::BaseObject *skyboxObj = GameEngine::currentLevel->CreateObject();
    Component::MeshRender* skyboxMeshRenderer = skyboxObj->AddComponent<Component::MeshRender>()
        ->SetMaterial(materialLibrary->GetMaterial("skybox"))
        ->SetMesh(cube);
    GameEngine::currentLevel->SetSkybox(skyboxMeshRenderer);
    GL::Shader::LogGLErrors("After Skybox Creation");

    // Planet obj
    constexpr float planetScale = 2.0f;
    Material *planetMat = materialLibrary->CreateMaterial("planet", planetShader);
    Material *atmosphereMat = materialLibrary->CreateMaterial("atmosphere", atmosphereShader);

    planetMat
        ->SetTransparency(Object::Material::Transparency::Opaque)
        ->attributes = Object::Material::RenderAttributes::Default;
    atmosphereMat
        ->SetTransparency(Object::Material::Transparency::Transparent)
        ->attributes = 
            Object::Material::RenderAttributes::Transform | Object::Material::RenderAttributes::NormalMatrix | 
            Object::Material::RenderAttributes::ReverseFaceCulling | Object::Material::RenderAttributes::OriginPos;


    Object::BaseObject *planet = GameEngine::currentLevel->CreateObject();
    planet->AddComponent<Component::Transform>()
        ->SetScale(glm::vec3(planetScale))
        ->ForceUpdateMatrixTransform();

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

    planet->AddComponent<Component::GravityGenerator>()->strength = 5000.0f;

    planetGenComp->PlanetifyMesh(rand());
    // -----

    // model import
    Object::GameObject *modelObj = GameEngine::currentLevel->CreateGameObject();
    modelObj->GetTransform()
        ->SetPos(glm::vec3(2.2f, 0.0f, 2.2f) * planetScale)
        ->SetScale(glm::vec3(0.25f))
        ->SetRot(glm::vec3(0.0f, -90.0f, 0.0f))
        ->ForceUpdateMatrixTransform();

    Component::MeshRender *modelRenderComp = modelObj->GetRenderComponent();
    std::shared_ptr<GL::Model> modelMesh = std::make_shared<GL::Model>("Models/teapot.obj");
    modelRenderComp->SetMesh(modelMesh);
    Component::ConvexHullCollider *modelCollider = modelObj->AddComponent<Component::ConvexHullCollider>();
    modelCollider->Generate(*modelMesh->GetMeshes()[0], Physics::Layer::Dynamic);

    modelObj->AddComponent<Component::GravityForce>();

    GL::Shader::LogGLErrors("After Model Loading");

    // floor
    Object::GameObject *floor = GameEngine::currentLevel->CreateGameObject();
    floor->GetTransform()
        ->SetScale(glm::vec3(80.0f, 0.1f, 80.0f))
        ->SetPos(glm::vec3(0.0f, -2.0f, 0.0f))
        ->ForceUpdateMatrixTransform();
    Component::MeshRender *floorRenderComp = floor->GetRenderComponent();
    floorRenderComp->SetMesh(cube);

    //floor->AddComponent<Component::BoxCollider>()
    //    ->Generate(glm::vec3(1.0f), Physics::Layer::Static);

    // lights
    Object::BaseObject *lightObj = GameEngine::currentLevel->CreateLightObject(Math::RGB(255, 0, 0), 3.5f);
    lightObj->GetComponent<Component::Transform>()->SetPos(glm::vec3(1.0f, -1.2f, 2.5f));
    lightObj->GetComponent<Component::MeshRender>()
        ->SetMesh(cube)
        ->SetMaterial(materialLibrary->GetMaterial("red"));

    Object::BaseObject *lightObj2 = GameEngine::currentLevel->CreateLightObject(Math::RGB(255, 255, 255), 2.5f);
    lightObj2->GetComponent<Component::Transform>()->SetPos(glm::vec3(0.0f, -1.2f, 2.5f));
    lightObj2->GetComponent<Component::MeshRender>()
        ->SetMesh(cube)
        ->SetMaterial(materialLibrary->GetMaterial("white"));

    Object::GameObject *shadowCaster = GameEngine::currentLevel->CreateGameObject();
    shadowCaster->GetTransform()
        ->SetPos(glm::vec3(0.5f, -1.8f, 2.8f))
        ->SetScale(glm::vec3(0.3f));
    shadowCaster->GetRenderComponent()->SetMesh(cube);

    lightObj->Disable();
    lightObj2->Disable();
    shadowCaster->Disable();
    floor->Disable();
    //modelObj->Disable();
    GL::Shader::LogGLErrors("After Creating Lights");

    // Camera obj
    Object::BaseObject *camObj = GameEngine::currentLevel->CreateObject();
    camObj->AddComponent<Component::Transform>()->SetPos(glm::vec3(0.0f, 0.5f, 2.5f));
    camObj->AddComponent<Component::Camera>();
    camObj->AddComponent<Component::Movement>();

    // makes the player a point light source
    //Component::PointLightSource *pointLight = camObj->AddComponent<Component::PointLightSource>();
    //Component::PointLightSource::PointLightData lightData;
    //lightData.color = glm::vec3(1.0f, 1.0f, 1.0f);
    //pointLight->SetLightData(lightData);

    // makes the player a collider
    Component::SphereCollider *playerCollider = camObj->AddComponent<Component::SphereCollider>();
    playerCollider->Generate(0.7f, Physics::Layer::Kinematic);

    GL::Shader::LogGLErrors("After Object Creation");
    // ---------
}