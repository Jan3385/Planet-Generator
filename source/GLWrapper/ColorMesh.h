#pragma once

#include "GLWrapper/Mesh.h"

namespace GL{
class ColorMesh : public Mesh{
public:
    std::vector<glm::vec3> vertexColors;
    ColorMesh() = default;
    ColorMesh(GL::Mesh mesh);
    ColorMesh(std::vector<GL::VertexObj> vertices, std::vector<unsigned int> indices, std::vector<GL::TextureObj> textures, std::vector<glm::vec3> vertexColors);
    ~ColorMesh() = default;

    // Copy & move
    ColorMesh(const ColorMesh&other);
    ColorMesh& operator=(const ColorMesh&other);
    ColorMesh(ColorMesh&&other) noexcept;
    ColorMesh& operator=(ColorMesh&&other) noexcept;

    void UpdateMeshBuffers();
protected:
    void SetupMeshBuffers();
private:
    GL::Buffer<glm::vec3, GL::BufferTarget::ArrayBuffer> colorBuffer;
};
}

