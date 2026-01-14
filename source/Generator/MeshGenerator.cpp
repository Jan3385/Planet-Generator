#include "MeshGenerator.h"

#include <glm/gtc/constants.hpp>

glm::vec3 CalculateNormal(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3)
{
    return glm::normalize(glm::cross(v2 - v1, v3 - v1));
}
glm::vec2 CalculateUVCube(
    const glm::vec3& pos,
    const glm::vec3& normal)
{
    glm::vec3 n = glm::abs(normal);
    glm::vec2 uv;

    if (n.x >= n.y && n.x >= n.z) uv = { pos.z, pos.y };

    else if (n.y >= n.x && n.y >= n.z) uv = { pos.x, pos.z };

    else uv = { pos.x, pos.y };


    return uv * 0.5f + 0.5f;
}
glm::vec2 CalculateUVSpherical(const glm::vec3 &vertex)
{
    float u = 0.5f + (atan2(vertex.z, vertex.x) / (2.0f * glm::pi<float>()));
    float v = 0.5f - (asin(vertex.y) / glm::pi<float>());
    return glm::vec2(u, v);
}

std::vector<float> MeshGenerator::GenerateCubeVerticesValues()
{
    return std::vector<float>{
        -0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,  
        0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f,  
        0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f,  0.5f, -0.5f, 
        0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
    };
}

GL::Mesh MeshGenerator::GenerateCubeMesh()
{
    GL::Mesh mesh;

    std::vector<float> vertices = MeshGenerator::GenerateCubeVerticesValues();

    for (size_t i = 0; i < vertices.size(); i += 9) {
        glm::vec3 v0(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
        glm::vec3 v1(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
        glm::vec3 v2(vertices[i + 6], vertices[i + 7], vertices[i + 8]);

        glm::vec3 normal = CalculateNormal(v0, v1, v2);

        glm::vec3 positions[3] = { v0, v1, v2 };

        for (int j = 0; j < 3; ++j) {
            GL::VertexObj vertex;
            vertex.position = positions[j];
            vertex.normal = normal;
            vertex.uv = CalculateUVCube(positions[j], normal);

            mesh.vertices.push_back(vertex);
            mesh.indices.push_back(static_cast<unsigned int>(mesh.indices.size()));
        }
    }

    return mesh;
}
