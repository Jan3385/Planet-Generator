#include "BaseMeshRenderComponent.h"

glm::vec3 Component::BaseMeshRender::GetPosition() const
{
    if(!this->transform) return glm::vec3(0.0f);

    return this->transform->GetPos();
}

bool Component::BaseMeshRender::IsSphereInsideFrustum(const std::array<glm::vec4, 6> &frustumPlanes, glm::vec3 &centroid, double radius)
{
    for (const auto& plane : frustumPlanes) {
        if (glm::dot(glm::vec3(plane), centroid) + plane.w < -radius) {
            return false;
        }
    }

    return true;
}
