#include "GravityGeneratorComponent.h"

#include "Object/BaseObject.h"

std::vector<Component::GravityGenerator*> Component::GravityGenerator::gravitySourceInstances;

glm::vec3 Component::GravityGenerator::GetPos() const
{
    Debug::Assert(this->transform != nullptr, "GravityGenerator has no Transform component assigned");
    return this->transform->GetPos();
}

glm::vec3 Component::GravityGenerator::GetGravityAt(const glm::vec3 &point)
{
    glm::vec3 total = glm::vec3(0.0f);

    for(const auto& source : GravityGenerator::gravitySourceInstances){
        glm::vec3 dir = source->GetPos() - point;
        float dist = glm::length(dir);

        if(dist > 0.001f) {
            total += source->strength * dir / (dist * dist);
        }
    }
    return total;
}

void Component::GravityGenerator::Awake()
{
    this->transform = this->GetOwner()->GetComponent<Component::Transform>();
    this->gravitySourceInstances.push_back(this);
}

void Component::GravityGenerator::OnDestroy()
{
    auto it = std::find(this->gravitySourceInstances.begin(), this->gravitySourceInstances.end(), this);
    if(it != this->gravitySourceInstances.end()) {
        this->gravitySourceInstances.erase(it);
    }
}

void Component::GravityGenerator::OnEnable()
{
    this->gravitySourceInstances.push_back(this);
}

void Component::GravityGenerator::OnDisable()
{
    auto it = std::find(this->gravitySourceInstances.begin(), this->gravitySourceInstances.end(), this);
    if(it != this->gravitySourceInstances.end()) {
        this->gravitySourceInstances.erase(it);
    }
}
