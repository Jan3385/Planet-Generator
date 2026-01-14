#include "SimpleMeshComponent.h"

Component::SimpleMesh::~SimpleMesh()
{
    this->verticies.clear();
}

void Component::SimpleMesh::SetMeshData(std::vector<float> verticies)
{
    this->verticies = verticies;

    this->NotifyMeshUpdated();
}
