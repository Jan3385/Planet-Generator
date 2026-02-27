#pragma once

#include <glad/glad.h>

#include "GLWrapper/Shader.h"
#include "GLWrapper/VertexArray.h"
#include "GLWrapper/Texture.h"

namespace GL
{
enum class DepthBufferMode : uint8_t{
    None = 0,
    RenderBuffer = 1,
    Texture = 2
};
class FrameBuffer{
public:
    FrameBuffer(DepthBufferMode mode);
    ~FrameBuffer();

    void AddBufferTexture(GLenum internalFormat, GL::TextureFormat format, GLenum type);
    void CompleteSetup();

    void BindShaderFBO() const;
    void BindTextures() const;
    void BindTextures(uint8_t start) const;
    void BindTextureTo(uint8_t attachmentIndex, uint8_t unit) const;
    void UnbindShaderFBO() const;

    void CopyDepthToFBO(GLuint targetFBO) const;
    void CopyDepthToFBO(FrameBuffer &targetFBO) const;

    // disable copy semantics
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    // move semantics
    FrameBuffer(FrameBuffer&&other) noexcept;
    FrameBuffer& operator=(FrameBuffer&&other) noexcept;

    void UpdateSize(const glm::uvec2& newSize);

    bool HasDepthBuffer() const { return depthStorage != 0; };
    DepthBufferMode GetDepthBufferType() const { return this->depthBufferType; };
protected:
    GLuint FBO = 0;

    DepthBufferMode depthBufferType = DepthBufferMode::None;
    GLuint depthStorage = 0;

    std::vector<GL::Texture*> attachments;

    glm::uvec2 size = glm::uvec2(800, 600);
};
}