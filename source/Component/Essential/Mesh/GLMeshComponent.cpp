#include "GLMeshComponent.h"

#include "Generator/MeshGenerator.h"

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

void Component::GLMesh::SetVerticies(const std::vector<float> &verticies)
{
    this->mesh = MeshGenerator::GenerateMeshFromVerticesValues(verticies);
}

std::vector<unsigned int> Component::GLMesh::GetIndices() const
{
    return this->mesh.indices;
}

void Component::GLMesh::SetGLVerticies(const std::vector<GL::VertexObj> &verticies)
{
    if(this->mesh.vertices.size() != verticies.size()){
        Debug::LogWarn(std::format(
            "GLMeshComponent::SetGLVerticies - Vertex count mismatch [{} -> {}]", this->mesh.vertices.size(), verticies.size()
        ));
    }

    this->mesh.vertices = verticies;
    this->mesh.UpdateMeshBuffers();
}

Component::GLMesh *Component::GLMesh::SetMeshData(const GL::Mesh &mesh)
{
    this->mesh = mesh;
    return this;
}
