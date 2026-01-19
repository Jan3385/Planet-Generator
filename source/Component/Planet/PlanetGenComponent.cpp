#include "PlanetGenComponent.h"

#include "Object/BaseObject.h"
#include "Generator/NoiseGenerator.h"

void Component::PlanetGen::PlanetifyMesh(uint32_t seed)
{
    GL::ColorMesh *mesh = renderComponent->GetMesh();
    std::vector<GL::VertexObj> vertices = mesh->vertices;
    std::vector<glm::vec3> colors(vertices.size());

    Generator::ValueNoise noise(seed);

    for(size_t i = 0; i < vertices.size(); ++i) {
        glm::vec3 normal = glm::normalize(vertices[i].position);
        float height = noise.GetNoise(normal* 3.0f) * 0.1f;

        colors[i] = glm::vec3(0.2f, 0.7f, 0.2f) + glm::vec3(height * 5.0f);
            
        vertices[i].position += normal * height;
    }

    mesh->vertices = std::move(vertices);
    mesh->vertexColors = std::move(colors);
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
    this->transform->RotateBy(glm::vec3(0.2f, 0.4f, 0.06f));
}

void Component::PlanetGen::FixedUpdate()
{
}
