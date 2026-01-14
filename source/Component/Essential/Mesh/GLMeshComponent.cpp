#include "GLMeshComponent.h"

bool Component::GLMesh::IsEmpty() const
{
    return  this->mesh.vertices.empty() && 
            this->mesh.indices.empty() && 
            this->mesh.textures.empty();
}

std::vector<float> Component::GLMesh::GetVerticies() const
{
    std::vector<float> verticies;
    verticies.reserve(this->mesh.vertices.size() * 3);

    for (const auto& vertex : this->mesh.vertices) {
        verticies.push_back(vertex.position.x);
        verticies.push_back(vertex.position.y);
        verticies.push_back(vertex.position.z);
    }

    return verticies;
}

std::vector<unsigned int> Component::GLMesh::GetIndices() const
{
    return this->mesh.indices;
}

void Component::GLMesh::SetMeshData(const GL::Mesh &mesh)
{
    this->mesh = mesh;
}
