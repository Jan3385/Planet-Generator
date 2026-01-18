#include "PlanetGenComponent.h"

#include "Object/BaseObject.h"

void Component::PlanetGen::PlanetifyMesh(uint32_t seed)
{
    std::vector<GL::VertexObj> vertices = mesh->GetGLVerticies();

    for(auto& vertex : vertices){
        glm::vec3 normal = glm::normalize(vertex.position);
        float height = 
            0.1f * (
                1.0f + 
                0.5f * static_cast<float>(std::sin(seed + normal.x * 10.0f)) +
                0.5f * static_cast<float>(std::sin(seed + normal.y * 15.0f)) +
                0.5f * static_cast<float>(std::sin(seed + normal.z * 20.0f))
            );
            
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
