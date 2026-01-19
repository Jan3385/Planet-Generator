#include "PlanetGenComponent.h"

#include "Object/BaseObject.h"
#include "Generator/NoiseGenerator.h"

void Component::PlanetGen::PlanetifyMesh(uint32_t seed)
{
    std::vector<GL::VertexObj> vertices = renderComponent->GetMesh()->vertices;

    Generator::ValueNoise noise(seed);

    for(auto& vertex : vertices){
        glm::vec3 normal = glm::normalize(vertex.position);
        float height = noise.GetNoise(normal* 3.0f) * 0.1f;
            
        vertex.position += normal * height;
    }

    renderComponent->GetMesh()->vertices = vertices;
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
