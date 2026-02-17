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

    virtual void Bind() const;
    virtual void Draw() const { glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0); };

    virtual void SetupMeshBuffers();
    virtual void UpdateMeshBuffers();

    double GetFrustumRadiusWithCentroid(glm::vec3 *centroidOut, glm::vec3 position, glm::vec3 scale) const;
protected:
    glm::vec3 centroid = glm::vec3(0.0f);
    double frustumRadius = 0.0;
    static glm::vec3 CalculateCentroid(const std::vector<VertexObj>& vertices);
    static double CalculateFrustumRadius(const std::vector<VertexObj>& vertices, glm::vec3 centroid);

    bool buffersInitialized = false;

    GL::Buffer<GL::VertexObj, GL::BufferTarget::ArrayBuffer> vertexBuffer;
    GL::Buffer<unsigned int, GL::BufferTarget::ElementArrayBuffer> indexBuffer;
    GL::VertexArray vertexArrayObject;
};
}

