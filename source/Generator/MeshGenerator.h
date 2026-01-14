#pragma once

#include "Component/Essential/Mesh/GLMeshComponent.h"

namespace MeshGenerator {
std::vector<float> GenerateCubeVerticesValues();
GL::Mesh GenerateCubeMesh();
}