#include "MeshColliderComponent.h"

#include <Jolt/Physics/Collision/Shape/MeshShape.h>

void Component::MeshCollider::Generate(const GL::Mesh &mesh, Physics::Layer layer, const glm::vec3 &offset, const glm::quat &rotation)
{
    JPH::EMotionType motionType = Physics::GetMotionType(layer);
    Debug::Assert(motionType == JPH::EMotionType::Static, "MeshColliderComponent: Mesh colliders can only be static, non-static layer provided");


    if(!this->transform){
        Debug::LogError("MeshColliderComponent: Cannot generate collider without a TransformComponent");
        return;
    }

    glm::vec3 scale = this->transform->GetScale();

    JPH::TriangleList triangles;

    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        glm::vec3 v0 = mesh.vertices[mesh.indices[i]].position;
        glm::vec3 v1 = mesh.vertices[mesh.indices[i + 1]].position;
        glm::vec3 v2 = mesh.vertices[mesh.indices[i + 2]].position;

        v0 *= scale;
        v1 *= scale;
        v2 *= scale;

        triangles.emplace_back(JPH::Vec3(v0.x, v0.y, v0.z), JPH::Vec3(v1.x, v1.y, v1.z), JPH::Vec3(v2.x, v2.y, v2.z));
    }

    JPH::MeshShapeSettings settings(triangles);

    JPH::ShapeSettings::ShapeResult result = settings.Create();
    Debug::Assert(result.IsValid(), "MeshColliderComponent: Failed to create mesh shape");

    JPH::ShapeRefC shape = result.Get();

    shape = ApplyOffset(shape, offset, rotation);

    glm::vec3 pos = this->transform->GetPos();
    glm::quat rot = this->transform->GetRot();
    this->bodyID = this->CreateBody(shape, pos, rot, layer);
}