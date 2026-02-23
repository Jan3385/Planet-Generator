#pragma once

#include "GLWrapper/Mesh.h"

namespace MeshGenerator {
glm::vec3 CalculateNormal(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3);

std::shared_ptr<GL::Mesh> GenerateMeshFromVerticesValues(const std::vector<float>& verticesValues);

std::vector<float> GenerateCubeVerticesValues();
std::vector<float> GenerateSpherifiedCubeVerticesValues(int subdivisions);
std::shared_ptr<GL::Mesh> GenerateCubeMesh();
std::shared_ptr<GL::Mesh> GenerateSpherifiedCubeMesh(int subdivisions);
}