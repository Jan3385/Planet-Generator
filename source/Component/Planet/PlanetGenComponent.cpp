#include "PlanetGenComponent.h"

#include "Object/BaseObject.h"
#include "Generator/NoiseGenerator.h"

void Component::PlanetGen::PlanetifyMesh(uint32_t seed)
{
    std::vector<GL::VertexObj> vertices = mesh->GetGLVerticies();

    Generator::ValueNoise noise(seed);

    for(auto& vertex : vertices){
        glm::vec3 normal = glm::normalize(vertex.position);
        float height = noise.GetNoise(normal* 3.0f) * 0.1f;
            
        vertex.position += normal * height;
    }

    mesh->SetGLVerticies(vertices);
}

void Component::PlanetGen::Awake()
{
    transform = GetOwner()->GetComponent<Component::Transform>();
    mesh = GetOwner()->GetComponent<Component::GLMesh>();
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
}

void Component::PlanetGen::FixedUpdate()
{
}
