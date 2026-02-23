#include "Mesh.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"

GL::Mesh::Mesh(std::vector<GL::VertexObj> vertices, std::vector<unsigned int> indices, std::vector<GL::TextureObj> textures)
    : vertices(vertices), indices(indices), textures(textures), 
      centroid(GL::Mesh::CalculateCentroid(vertices)), frustumRadius(GL::Mesh::CalculateFrustumRadius(vertices, this->centroid))
{
    this->SetupMeshBuffers();
}

// Copy
GL::Mesh::Mesh(const GL::Mesh &other)
    : vertices(other.vertices), indices(other.indices), textures(other.textures), 
      centroid(other.centroid), frustumRadius(other.frustumRadius)
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
        this->centroid = other.centroid;
        this->frustumRadius = other.frustumRadius;
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
    this->centroid = other.centroid;
    this->frustumRadius = other.frustumRadius;
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
        this->centroid = other.centroid;
        this->frustumRadius = other.frustumRadius;
    }
    return *this;
}

void GL::Mesh::Draw() const
{
    //TODO: Bind textures ?

    this->vertexArrayObject.Bind();
    this->indexBuffer.Bind();
    
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0); 
}

void GL::Mesh::UpdateMeshBuffers()
{
    this->vertexBuffer.SetData(this->vertices, GL_STATIC_DRAW);
    this->indexBuffer.SetData(this->indices, GL_STATIC_DRAW);

    this->centroid = GL::Mesh::CalculateCentroid(this->vertices);
    this->frustumRadius = GL::Mesh::CalculateFrustumRadius(this->vertices, this->centroid);
}

/// @brief Returns the frustum radius along with the centroid of the mesh in world space
/// @param centroidOut output pointer for the centroid in world space
/// @param position world position of the mesh
/// @param scale world scale of the mesh
/// @return the frustum radius of the mesh
double GL::Mesh::GetFrustumRadiusWithCentroid(glm::vec3 *centroidOut, glm::vec3 position, glm::vec3 scale) const
{
    float maxScale = std::max(scale.x, std::max(scale.y, scale.z));
    centroidOut->x = this->centroid.x * scale.x + position.x;
    centroidOut->y = this->centroid.y * scale.y + position.y;
    centroidOut->z = this->centroid.z * scale.z + position.z;
    return this->frustumRadius * maxScale;
}

glm::vec3 GL::Mesh::CalculateCentroid(const std::vector<VertexObj>& vertices)
{
    glm::vec3 sum(0.0f);
    for (const auto& vertex : vertices) {
        sum += vertex.position;
    }

    return sum / static_cast<float>(vertices.size());
}

double GL::Mesh::CalculateFrustumRadius(const std::vector<VertexObj>& vertices, glm::vec3 centroid)
{
    double maxDistanceSquared = 0.0;

    for (const auto& vertex : vertices) {
        double distanceSquared = glm::distance2(vertex.position, centroid);
        if (distanceSquared > maxDistanceSquared) {
            maxDistanceSquared = distanceSquared;
        }
    }

    return std::sqrt(maxDistanceSquared);
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

    this->buffersInitialized = true;

    this->centroid = GL::Mesh::CalculateCentroid(this->vertices);
    this->frustumRadius = GL::Mesh::CalculateFrustumRadius(this->vertices, this->centroid);
}