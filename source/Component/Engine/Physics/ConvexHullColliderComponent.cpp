#include "ConvexHullColliderComponent.h"

#include <Jolt/Math/Vec3.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>

void Component::ConvexHullCollider::Generate(std::vector<JPH::Vec3> points, Physics::Layer layer, const glm::vec3 &offset, const glm::quat &rotation)
{
    if(!this->transform){
        Debug::LogError("ConvexHullColliderComponent: Cannot generate collider without a TransformComponent");
        return;
    }

    glm::vec3 scale = this->transform->GetScale();

    for(auto& point : points){
        point.SetX(point.GetX() * scale.x);
        point.SetY(point.GetY() * scale.y);
        point.SetZ(point.GetZ() * scale.z);
    }

    JPH::ConvexHullShapeSettings settings(points.data(), points.size());
    JPH::ShapeSettings::ShapeResult result = settings.Create();
    Debug::Assert(result.IsValid(), "ConvexHullColliderComponent: Failed to create convex hull shape");

    JPH::ShapeRefC shape = result.Get();

    shape = ApplyOffset(shape, offset, rotation);

    glm::vec3 pos = this->transform->GetPos();
    glm::quat rot = this->transform->GetRot();
    this->bodyID = this->CreateBody(shape, pos, rot, layer);
}

void Component::ConvexHullCollider::Generate(std::vector<glm::vec3> points, Physics::Layer layer, const glm::vec3 &offset, const glm::quat &rotation)
{
    std::vector<JPH::Vec3> joltPoints(points.size());
    for(size_t i = 0; i < points.size(); ++i){
        joltPoints[i] = JPH::Vec3(points[i].x, points[i].y, points[i].z);
    }

    this->Generate(joltPoints, layer, offset, rotation);
}

void Component::ConvexHullCollider::Generate(const GL::Mesh &mesh, Physics::Layer layer, const glm::vec3 &offset, const glm::quat &rotation)
{
    std::vector<glm::vec3> points(mesh.vertices.size());
    for (size_t i = 0; i < mesh.vertices.size(); ++i) {
        points[i] = mesh.vertices[i].position;
    }
    
    this->Generate(points, layer, offset, rotation);
}
