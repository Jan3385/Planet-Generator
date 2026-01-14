#include "MeshGenerator.h"

#include <glm/gtc/constants.hpp>
#include <unordered_map>

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

// generating indices
constexpr int KEY_SCALE = 10000;
struct VertexKey {
    int px, py, pz;
    int nx, ny, nz;
    int u, v;

    bool operator==(const VertexKey& other) const {
        return px == other.px && py == other.py && pz == other.pz &&
               nx == other.nx && ny == other.ny && nz == other.nz &&
               u == other.u && v == other.v;
    }
};
VertexKey GenerateVertexKey(const GL::VertexObj &vertex) {
    VertexKey key;
    key.px = static_cast<int>(vertex.position.x * KEY_SCALE);
    key.py = static_cast<int>(vertex.position.y * KEY_SCALE);
    key.pz = static_cast<int>(vertex.position.z * KEY_SCALE);
    key.nx = static_cast<int>(vertex.normal.x * KEY_SCALE);
    key.ny = static_cast<int>(vertex.normal.y * KEY_SCALE);
    key.nz = static_cast<int>(vertex.normal.z * KEY_SCALE);
    key.u  = static_cast<int>(vertex.uv.x * KEY_SCALE);
    key.v  = static_cast<int>(vertex.uv.y * KEY_SCALE);
    return key;
}
struct VertexKeyHash{
    size_t operator()(const VertexKey &key) const {
        size_t h = 0;

        // Evil hash math
        auto hash_combine = [&h](int v) {
            h ^= std::hash<int>()(v) + 0x9e3779b9 + (h << 6) + (h >> 2);
        };

        hash_combine(key.px); hash_combine(key.py); hash_combine(key.pz);
        hash_combine(key.nx); hash_combine(key.ny); hash_combine(key.nz);
        hash_combine(key.u);  hash_combine(key.v);

        return h;
    }
};
void DeduplicateVertices(
    std::vector<GL::VertexObj> &IN_Vertices,
    std::vector<GL::VertexObj> &OUT_Vertices,
    std::vector<unsigned int>  &OUT_Indices
){
    std::unordered_map<VertexKey, uint32_t, VertexKeyHash> vertexMap;
    
    for(const GL::VertexObj &vertex : IN_Vertices) {
        VertexKey key = GenerateVertexKey(vertex);

        auto it = vertexMap.find(key);
        if(it != vertexMap.end()) {
            // Vertex already exists
            OUT_Indices.push_back(it->second);
        } else {
            // New vertex
            uint32_t newIndex = static_cast<uint32_t>(OUT_Vertices.size());
            OUT_Vertices.push_back(vertex);
            OUT_Indices.push_back(newIndex);
            vertexMap[key] = newIndex;
        }
    }
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
        }
    }

    std::vector<GL::VertexObj> dedupedVertices;
    DeduplicateVertices(mesh.vertices, dedupedVertices, mesh.indices);
    mesh.vertices = std::move(dedupedVertices);

    return mesh;
}
