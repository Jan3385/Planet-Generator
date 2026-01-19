#include "ColorMesh.h"

GL::ColorMesh::ColorMesh(std::vector<GL::VertexObj> vertices, std::vector<unsigned int> indices, std::vector<GL::TextureObj> textures, std::vector<glm::vec3> vertexColors)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->vertexColors = vertexColors;

    this->SetupMeshBuffers();
}

GL::ColorMesh::ColorMesh(const GL::ColorMesh &other)
{
    this->vertices = other.vertices;
    this->indices = other.indices;
    this->textures = other.textures;
    this->vertexColors = other.vertexColors;

    this->SetupMeshBuffers();
}

GL::ColorMesh &GL::ColorMesh::operator=(const GL::ColorMesh &other)
{
    if (this != &other) {
        this->vertices = other.vertices;
        this->indices = other.indices;
        this->textures = other.textures;
        this->vertexColors = other.vertexColors;

        this->SetupMeshBuffers();
    }
    return *this;
}

GL::ColorMesh::ColorMesh(GL::ColorMesh &&other) noexcept
{
    this->vertices = std::move(other.vertices);
    this->indices = std::move(other.indices);
    this->textures = std::move(other.textures);
    this->vertexColors = std::move(other.vertexColors);
    this->colorBuffer = std::move(other.colorBuffer);
}

GL::ColorMesh &GL::ColorMesh::operator=(GL::ColorMesh &&other) noexcept
{
    if (this != &other) {
        this->vertices = std::move(other.vertices);
        this->indices = std::move(other.indices);
        this->textures = std::move(other.textures);
        this->vertexColors = std::move(other.vertexColors);
        this->colorBuffer = std::move(other.colorBuffer);
    }
    return *this;
}

void GL::ColorMesh::UpdateMeshBuffers()
{
    Mesh::UpdateMeshBuffers();
    this->colorBuffer.SetData(this->vertexColors, GL_STATIC_DRAW);
}

void GL::ColorMesh::SetupMeshBuffers()
{
    this->vertexBuffer = GL::Buffer<GL::VertexObj, GL::BufferTarget::ArrayBuffer>("Mesh Vertex Buffer");
    this->vertexBuffer.SetData(this->vertices, GL_STATIC_DRAW);
    this->indexBuffer = GL::Buffer<unsigned int, GL::BufferTarget::ElementArrayBuffer>("Mesh Index Buffer");
    this->indexBuffer.SetData(this->indices, GL_STATIC_DRAW);
    this->colorBuffer = GL::Buffer<glm::vec3, GL::BufferTarget::ArrayBuffer>("Mesh Color Buffer");
    this->colorBuffer.SetData(this->vertexColors, GL_STATIC_DRAW);
    this->vertexArrayObject = GL::VertexArray("Mesh Vertex Array Object");

    vertexArrayObject.Bind();
    vertexArrayObject.AddAttribute<glm::vec3, GL::VertexObj>(0, 3, this->vertexBuffer, GL_FALSE, offsetof(GL::VertexObj, position));
    vertexArrayObject.AddAttribute<glm::vec3, GL::VertexObj>(1, 3, this->vertexBuffer, GL_FALSE, offsetof(GL::VertexObj, normal));
    vertexArrayObject.AddAttribute<glm::vec3, glm::vec3>(2, 3, this->colorBuffer, GL_FALSE, 0);
    vertexArrayObject.Unbind();
}
