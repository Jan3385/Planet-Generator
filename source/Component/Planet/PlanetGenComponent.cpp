#include "PlanetGenComponent.h"

#include "Object/BaseObject.h"
#include "Generator/NoiseGenerator.h"
#include "Math/Random.h"

void Component::PlanetGen::PlanetifyMesh(uint32_t seed)
{
    GL::ColorMesh *mesh = renderComponent->GetMesh();
    std::vector<GL::VertexObj> vertices = mesh->vertices;
    std::vector<glm::vec3> colors(vertices.size());

    Generator::ValueNoise noise(seed);

    for(size_t i = 0; i < vertices.size(); ++i) {
        glm::vec3 normal = glm::normalize(vertices[i].position);
        float height = noise.GetNoise(normal * 2.5f) * 0.1f;

        colors[i] = GetVertexColor(height, normal).ToVec3();
            
        vertices[i].position += normal * height;
    }

    mesh->vertices = std::move(vertices);
    mesh->vertexColors = std::move(colors);
    mesh->UpdateMeshBuffers();
}

Math::RGB Component::PlanetGen::GetVertexColor(float height, glm::vec3 position)
{
    uint32_t hash = Math::Random::HashFromPos(position, 95173);
    Math::RGB color;

    if(height < 0.02f) color = Math::RGB(80, 105, 229);
    else if(height < 0.04f) color = Math::RGB(236, 177, 89);
    else if(height < 0.06f) color = Math::RGB(100, 170, 60);
    else if(height < 0.08f) color = Math::RGB(80, 120, 50);
    else color = Math::RGB(200, 200, 200);
    
    color = color.Mix(
        Math::RGB(0,0,0),
        Math::Random::FloatFromHash(hash, 0.0f, 0.05f)
    );

    return color;
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
