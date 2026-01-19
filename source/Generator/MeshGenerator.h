#pragma once

#include "GLWrapper/Mesh.h"

namespace MeshGenerator {
GL::Mesh GenerateMeshFromVerticesValues(const std::vector<float>& verticesValues);

std::vector<float> GenerateCubeVerticesValues();
std::vector<float> GenerateSpherifiedCudeVerticesValues(int subdivisions);
GL::Mesh GenerateCubeMesh();
GL::Mesh GenerateSpherifiedCubeMesh(int subdivisions);
}