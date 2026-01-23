#include "PlanetGenComponent.h"

#include "Object/BaseObject.h"
#include "Generator/NoiseGenerator.h"
#include "Math/Random.h"

void Component::PlanetGen::PlanetifyMesh(uint32_t seed)
{
    GL::Mesh *mesh = renderComponent->GetMesh();
    std::vector<GL::VertexObj> vertices = mesh->vertices;

    Generator::ValueNoise noise(seed);

    for(size_t i = 0; i < vertices.size(); ++i) {
        glm::vec3 pos = vertices[i].position;
        pos *= noise.GetNoise(pos * 0.5f) + 1.0f;

        float height = noise.GetNoise(pos * 1.0f) * 1.0f;
        height = log2f(height + 1.2f)/8.0f;

        height += noise.GetNoise(pos * 5.0f) * 0.05f;
        height += noise.GetNoise(pos * 25.0f) * 0.03f;

        if(height < 0.02f) height = 0.02f;
            
        vertices[i].position += pos * height;
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
    this->transform->RotateBy(glm::vec3(0.2f, 0.4f, 0.06f));
}

void Component::PlanetGen::FixedUpdate()
{
}
