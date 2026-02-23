#include "PlanetGenComponent.h"

#include "Object/BaseObject.h"
#include "Generator/NoiseGenerator.h"
#include "Math/Random.h"
#include "Engine/Engine.h"

void Component::PlanetGen::PlanetifyMesh(uint32_t seed)
{
    Debug::LogTrace("PlanetGen: Planetifying mesh with seed " + std::to_string(seed));
    constexpr float desiredAvgHeight = 0.030f * PLANET_SCALE;

    GL::Mesh* mesh = renderComponent->GetMesh()->GetMeshes()[0];
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
    renderComponent = GetOwner()->GetComponent<Component::PlanetMeshRender>();
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
