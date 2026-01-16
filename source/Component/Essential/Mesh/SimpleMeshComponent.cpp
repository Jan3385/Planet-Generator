#include "SimpleMeshComponent.h"

Component::SimpleMesh::~SimpleMesh()
{
    this->verticies.clear();
}

Component::SimpleMesh* Component::SimpleMesh::SetMeshData(std::vector<float> verticies)
{
    this->verticies = verticies;

    this->NotifyMeshUpdated();
    return this;
}
