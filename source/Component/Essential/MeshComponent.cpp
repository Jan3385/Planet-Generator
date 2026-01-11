#include "MeshComponent.h"

Component::Mesh::~Mesh()
{
    verticies.clear();

    for(auto& callback : updateCallbacks){
        callback->OnMeshUpdated(this);
    }
}

void Component::Mesh::SetMeshData(std::vector<float> verticies, std::vector<float> normals)
{
    this->verticies = verticies;
    this->normals = normals;

    for(auto& callback : updateCallbacks){
        callback->OnMeshUpdated(this);
    }
}
