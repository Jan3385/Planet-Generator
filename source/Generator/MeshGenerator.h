#pragma once

#include "Component/Essential/Mesh/GLMeshComponent.h"

namespace MeshGenerator {
std::vector<float> GenerateCubeVerticesValues();
std::vector<float> GenerateSpherifiedCudeVerticesValues(int subdivisions);
GL::Mesh GenerateCubeMesh();
GL::Mesh GenerateSpherifiedCubeMesh(int subdivisions);
}