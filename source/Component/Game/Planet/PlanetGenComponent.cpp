#include "PlanetGenComponent.h"

#include "Object/BaseObject.h"
#include "Generator/NoiseGenerator.h"
#include "Generator/MeshGenerator.h"
#include "Math/Random.h"
#include "Engine/Engine.h"

void Component::PlanetGen::PlanetifyMesh(uint32_t seed)
{
    Debug::LogTrace("PlanetGen: Planetifying mesh with seed " + std::to_string(seed));
    constexpr float desiredAvgHeight = 0.030f * PLANET_SCALE;

    GL::Mesh* mesh = planetMesh.get();
    std::vector<GL::VertexObj> vertices = mesh->vertices;

    Generator::ValueNoise noise(seed);

    std::vector<float> heights(vertices.size());
    float heightSum = 0.0f;
    for(size_t i = 0; i < vertices.size(); ++i) {
        glm::vec3 pos = vertices[i].position;

        float height = noise.GetNoise(pos * 1.0f) * 0.18f;

        height += noise.GetNoise(pos * 5.0f) * 0.06f;
        height += noise.GetNoise(pos * 10.0f) * 0.05f;
        height += noise.GetNoise(pos * 25.0f) * 0.04f;

        if(height < 0.002f) height = 0.002f;
        heights[i] = height;
        heightSum += height;
    }
    float avgHeight = heightSum / static_cast<float>(vertices.size());
    float heightScale = log10(desiredAvgHeight / avgHeight + 1.0f);
    Debug::LogSpam("HeighScale for planet: " + std::to_string(heightScale));

    for(size_t i = 0; i < vertices.size(); ++i) {
        vertices[i].position = glm::normalize(vertices[i].position) * (heights[i] * heightScale + 1.0f);
    }

    mesh->vertices = std::move(vertices);
    mesh->UpdateMeshBuffers();

    this->planetCollider->Generate(*mesh, Physics::Layer::Kinematic);
}

void Component::PlanetGen::SetMaterials(std::string planetMatName, std::string atmosphereMatName)
{
    Object::Material* planetMat = GameEngine::instance->materialLibrary->GetMaterial(planetMatName);
    if(planetMat){
        planetMat = planetMat->Clone();
        planetMat->SetUniformBuffer("PlanetPalette", &this->planetPaletteBuffer, 0);
        this->planetRenderComponent->SetMaterial(planetMat);
    }
    else Debug::LogError("PlanetGen: Failed to set planet material, material not found: " + planetMatName);

    Object::Material* atmosphereMat = GameEngine::instance->materialLibrary->GetMaterial(atmosphereMatName);
    if(atmosphereMat){
        atmosphereMat = atmosphereMat->Clone();
        atmosphereMat->SetUniformBuffer("AtmospherePalette", &this->atmospherePaletteBuffer, 0);
        this->atmosphereRenderComponent->SetMaterial(atmosphereMat);
    }
    else Debug::LogError("PlanetGen: Failed to set atmosphere material, material not found: " + atmosphereMatName);
}

void Component::PlanetGen::SetAtmosphereColors(const glm::vec4 &horizonColor, const glm::vec4 &zenithColor)
{
    this->atmospherePaletteBuffer.SetData({horizonColor, zenithColor}, GL_STATIC_DRAW);
}

void Component::PlanetGen::SetPlanetColors(const MaterialSTD140 &deepOcean, const MaterialSTD140 &shallowOcean, const MaterialSTD140 &sand, const MaterialSTD140 &grass, const MaterialSTD140 &rock, const MaterialSTD140 &snow)
{
    planetPalette palette{deepOcean, shallowOcean, sand, grass, rock, snow};
    this->planetPaletteBuffer.SetData(palette, GL_STATIC_DRAW);
}

void Component::PlanetGen::ImGuiUpdate()
{
    ImGui::Begin("Planet Generator");
    if(ImGui::Button("Planetify Mesh")) {
        uint32_t seed = rand();
        this->PlanetifyMesh(seed);
    }
    ImGui::Checkbox("rotate", &rotatePlanet);
    ImGui::End();
}

void Component::PlanetGen::Awake()
{
    transform = GetOwner()->GetComponent<Component::Transform>();

    Object::BaseObject* child = GameEngine::currentLevel->CreateObject();
    child->SetParent(GetOwner());
    Transform* childTransform = child->AddComponent<Component::Transform>();
    childTransform->SetScale(glm::vec3(transform->GetScale() * (1 + PLANET_SCALE * 0.15f)));
    
    this->planetRenderComponent = GetOwner()->AddComponent<Component::MeshRender>();
    this->atmosphereRenderComponent = child->AddComponent<Component::MeshRender>();

    this->planetMesh = MeshGenerator::GenerateSpherifiedCubeMesh(60);
    this->atmosphereMesh = MeshGenerator::GenerateSpherifiedCubeMesh(10);

    this->atmospherePaletteBuffer = GL::Buffer<atmospherePalette, GL::BufferTarget::UniformBuffer>("AtmospherePaletteBuffer");
    this->planetPaletteBuffer = GL::Buffer<planetPalette, GL::BufferTarget::UniformBuffer>("PlanetPaletteBuffer");

    this->planetRenderComponent
        ->SetMesh(planetMesh);

    this->atmosphereRenderComponent
        ->SetMesh(atmosphereMesh);

    this->planetCollider = GetOwner()->AddComponent<Component::ConvexHullCollider>();
    planetCollider->SetTransform(transform);
}

void Component::PlanetGen::OnDestroy()
{
}

void Component::PlanetGen::OnEnable()
{
}

void Component::PlanetGen::OnDisable()
{
}

void Component::PlanetGen::Update()
{
    float deltaTime = GameEngine::instance->DeltaTime();
    if(this->rotatePlanet) this->transform->RotateBy(glm::vec3(16.0f, 32.0f, 6.0f) * deltaTime);
}

void Component::PlanetGen::FixedUpdate()
{
}
