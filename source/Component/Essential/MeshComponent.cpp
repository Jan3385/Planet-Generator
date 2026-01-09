#include "MeshComponent.h"

Component::Mesh::~Mesh()
{
    verticies.clear();
    indicies.clear();

    for(auto& callback : updateCallbacks){
        callback->OnMeshUpdated(this);
    }
}

void Component::Mesh::SetMeshData(std::vector<float> verticies, std::vector<unsigned int> indicies)
{
    this->verticies = verticies;
    this->indicies = indicies;

    for(auto& callback : updateCallbacks){
        callback->OnMeshUpdated(this);
    }
}
