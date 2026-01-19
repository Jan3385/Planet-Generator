#include "Mesh.h"

GL::Mesh::Mesh(std::vector<GL::VertexObj> vertices, std::vector<unsigned int> indices, std::vector<GL::TextureObj> textures)
    : vertices(vertices), indices(indices), textures(textures)
{
    this->SetupMeshBuffers();
}

// Copy
GL::Mesh::Mesh(const GL::Mesh &other)
    : vertices(other.vertices), indices(other.indices), textures(other.textures)
{
    Debug::LogSpam("Copied GL::Mesh");
    this->SetupMeshBuffers();
}

GL::Mesh &GL::Mesh::operator=(const GL::Mesh &other)
{
    Debug::LogSpam("Copied GL::Mesh");
    if (this != &other) {
        this->vertices = other.vertices;
        this->indices = other.indices;
        this->textures = other.textures;
        this->SetupMeshBuffers();
    }
    return *this;
}

// Move
GL::Mesh::Mesh(GL::Mesh &&other) noexcept
{
    Debug::LogSpam("Moved GL::Mesh");
    this->vertices = std::move(other.vertices);
    this->indices = std::move(other.indices);
    this->textures = std::move(other.textures);
    this->vertexBuffer = std::move(other.vertexBuffer);
    this->indexBuffer = std::move(other.indexBuffer);
    this->vertexArrayObject = std::move(other.vertexArrayObject);
}

GL::Mesh &GL::Mesh::operator=(GL::Mesh &&other) noexcept
{
    Debug::LogSpam("Moved GL::Mesh");
    if (this != &other) {
        this->vertices = std::move(other.vertices);
        this->indices = std::move(other.indices);
        this->textures = std::move(other.textures);
        this->vertexBuffer = std::move(other.vertexBuffer);
        this->indexBuffer = std::move(other.indexBuffer);
        this->vertexArrayObject = std::move(other.vertexArrayObject);
    }
    return *this;
}

void GL::Mesh::Bind() const
{
    //TODO: Bind textures ?

    this->vertexArrayObject.Bind();
    this->indexBuffer.Bind();
}

void GL::Mesh::UpdateMeshBuffers()
{
    this->vertexBuffer.SetData(this->vertices, GL_STATIC_DRAW);
    this->indexBuffer.SetData(this->indices, GL_STATIC_DRAW);
}

void GL::Mesh::SetupMeshBuffers()
{
    if(this->buffersInitialized){
        Debug::LogError("Mesh buffers are already initialized!");
        return;
    }

    this->vertexBuffer = GL::Buffer<GL::VertexObj, GL::BufferTarget::ArrayBuffer>("Mesh Vertex Buffer");
    this->vertexBuffer.SetData(this->vertices, GL_STATIC_DRAW);
    this->indexBuffer = GL::Buffer<unsigned int, GL::BufferTarget::ElementArrayBuffer>("Mesh Index Buffer");
    this->indexBuffer.SetData(this->indices, GL_STATIC_DRAW);
    this->vertexArrayObject = GL::VertexArray("Mesh Vertex Array Object");

    vertexArrayObject.Bind();
    vertexArrayObject.AddAttribute<glm::vec3, GL::VertexObj>(0, 3, this->vertexBuffer, GL_FALSE, offsetof(GL::VertexObj, position));
    vertexArrayObject.AddAttribute<glm::vec3, GL::VertexObj>(1, 3, this->vertexBuffer, GL_FALSE, offsetof(GL::VertexObj, normal));
    vertexArrayObject.AddAttribute<glm::vec2, GL::VertexObj>(2, 2, this->vertexBuffer, GL_FALSE, offsetof(GL::VertexObj, uv));
    vertexArrayObject.Unbind();
}