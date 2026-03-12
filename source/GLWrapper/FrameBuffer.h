#pragma once

#include <glad/glad.h>

#include "GLWrapper/Shader.h"
#include "GLWrapper/VertexArray.h"
#include "GLWrapper/Texture.h"

namespace GL
{
enum class DepthBufferMode : uint8_t{
    None = 0,
    RenderBuffer,
    Texture,
    Cubemap
};
class FrameBuffer{
public:
    FrameBuffer();
    FrameBuffer(DepthBufferMode mode);
    ~FrameBuffer();

    void AddBufferTexture2D(GLenum internalFormat, GL::TextureFormat format, GLenum type);
    void DisableDrawRead();
    void CompleteSetup();

    void BindShaderFBO() const;
    void BindTextures() const;
    void BindTextures(uint8_t start) const;
    void BindTextureTo(uint8_t attachmentIndex, uint8_t unit) const;
    static void UnbindShaderFBO();

    void CopyDepthToFBO(GLuint targetFBO) const;
    void CopyDepthToFBO(FrameBuffer &targetFBO) const;

    // disable copy semantics
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    // move semantics
    FrameBuffer(FrameBuffer&&other) noexcept;
    FrameBuffer& operator=(FrameBuffer&&other) noexcept;

    void UpdateSize(const glm::uvec2& newSize);
    glm::vec2 GetSize() const { return this->size; };

    bool HasDepthBuffer() const { return depthStorage != 0; };
    GLuint GetDepthStorageID() const { return depthStorage; };
    DepthBufferMode GetDepthBufferType() const { return this->depthBufferType; };
protected:
    GLuint FBO = 0;

    DepthBufferMode depthBufferType = DepthBufferMode::None;
    GLuint depthStorage = 0;

    std::vector<GL::Texture*> attachments;

    glm::uvec2 size = glm::uvec2(800, 600);
};
}