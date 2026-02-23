#pragma once

#include <vector>
#include <string>

#include "GLWrapper/Buffer.h"
#include "GLWrapper/VertexArray.h"
#include "GLWrapper/Texture.h"
#include "glm/glm.hpp"

namespace GL{
class Mesh;

class IMeshRenderable{
public:
    virtual void Draw() const = 0;

    virtual std::vector<GL::Mesh*> GetMeshes() = 0;
};
struct VertexObj{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};
struct TextureObj{
    GL::Texture texture;
    std::string type;

    TextureObj() = default;
    TextureObj(const TextureObj&other){
        this->texture = other.texture;
        this->type = other.type;
    };
    TextureObj& operator=(const TextureObj&other){
        if(this != &other){
            this->texture = other.texture;
            this->type = other.type;
        }
        return *this;
    };
    TextureObj(TextureObj&&) noexcept = default;
    TextureObj& operator=(TextureObj&&) noexcept = default;
};

/// @brief A class representing a 3D mesh formed by vertices, indices and textures
class Mesh : public IMeshRenderable {
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

    virtual void Draw() const override;

    virtual std::vector<GL::Mesh*> GetMeshes() override { return { this }; }

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

