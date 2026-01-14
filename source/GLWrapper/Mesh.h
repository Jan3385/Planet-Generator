#pragma once

#include <vector>
#include <string>

#include "GLWrapper/Buffer.h"
#include "GLWrapper/VertexArray.h"
#include "glm/glm.hpp"

namespace GL{
struct VertexObj{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};
struct TextureObj{
    unsigned int id;
    std::string type;
};
class Mesh{
public:
    std::vector<GL::VertexObj> vertices;
    std::vector<unsigned int> indices;
    std::vector<GL::TextureObj> textures;
    
    Mesh(std::vector<GL::VertexObj> vertices, std::vector<unsigned int> indices, std::vector<GL::TextureObj> textures)
        : vertices(vertices), indices(indices), textures(textures) { };
    ~Mesh() = default;

    void Bind() const;
private:
    GL::Buffer<glm::vec3, GL::BufferTarget::ArrayBuffer> vertexBuffer;
    GL::Buffer<unsigned int, GL::BufferTarget::ElementArrayBuffer> indexBuffer;
    GL::VertexArray vertexArrayObject;

    void UpdateMeshBuffers();
};
}

