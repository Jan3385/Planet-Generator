#include "PlanetGenComponent.h"

#include "Object/BaseObject.h"
#include "Generator/NoiseGenerator.h"
#include "Math/Random.h"

void Component::PlanetGen::PlanetifyMesh(uint32_t seed)
{
    Debug::LogTrace("PlanetGen: Planetifying mesh with seed " + std::to_string(seed));
    constexpr float desiredAvgHeight = 0.03f;

    GL::Mesh *mesh = renderComponent->GetMesh();
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

        if(height < 0.02f) height = 0.02f;
        heights[i] = height;
        heightSum += height;
    }
    float avgHeight = heightSum / static_cast<float>(vertices.size());
    float heightScale = desiredAvgHeight / avgHeight;
    Debug::LogSpam("HeighScale for planet: " + std::to_string(heightScale));

    for(size_t i = 0; i < vertices.size(); ++i) {
        vertices[i].position += vertices[i].position * (heights[i] * heightScale);
    }

    mesh->vertices = std::move(vertices);
    mesh->UpdateMeshBuffers();
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
    //this->transform->RotateBy(glm::vec3(0.2f, 0.4f, 0.06f));
}

void Component::PlanetGen::FixedUpdate()
{
}
