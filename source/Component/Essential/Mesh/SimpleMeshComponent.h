#pragma once

#include <vector>

#include "Component/Essential/Mesh/BaseMeshComponent.h"

namespace Component {
class SimpleMesh : public BaseMesh {
public:
    SimpleMesh(Object::BaseObject* owner) : BaseMesh(owner) {};
    ~SimpleMesh() override;

    std::vector<float> GetVerticies() const override {
        return verticies;
    }

    SimpleMesh* SetMeshData(std::vector<float> verticies);

protected:
    std::vector<float> verticies;
};
}