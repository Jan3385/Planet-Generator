#pragma once

#include "GLWrapper/Mesh.h"

namespace MeshGenerator {
std::shared_ptr<GL::Mesh> GenerateMeshFromVerticesValues(const std::vector<float>& verticesValues);

std::vector<float> GenerateCubeVerticesValues();
std::vector<float> GenerateSpherifiedCubeVerticesValues(int subdivisions);
std::shared_ptr<GL::Mesh> GenerateCubeMesh();
std::shared_ptr<GL::Mesh> GenerateSpherifiedCubeMesh(int subdivisions);
}