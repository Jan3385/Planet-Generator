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
    
    Mesh() = default;
    Mesh(std::vector<GL::VertexObj> vertices, std::vector<unsigned int> indices, std::vector<GL::TextureObj> textures);
    ~Mesh() = default;

    // Copy & move
    Mesh(const Mesh&);
    Mesh& operator=(const Mesh&);
    Mesh(Mesh&&) noexcept;
    Mesh& operator=(Mesh&&) noexcept;

    void Bind() const;

    void UpdateMeshBuffers();
protected:
    void SetupMeshBuffers();
private:
    GL::Buffer<GL::VertexObj, GL::BufferTarget::ArrayBuffer> vertexBuffer;
    GL::Buffer<unsigned int, GL::BufferTarget::ElementArrayBuffer> indexBuffer;
    GL::VertexArray vertexArrayObject;
};
}

