#include "BaseMeshComponent.h"

Component::BaseMesh::~BaseMesh()
{
    for(auto& callback : updateCallbacks){
        callback->OnMeshUpdated(this);
    }
}