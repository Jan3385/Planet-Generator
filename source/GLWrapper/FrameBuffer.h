#pragma once

#include <glad/glad.h>

#include "GLWrapper/Shader.h"
#include "GLWrapper/VertexArray.h"
#include "GLWrapper/Texture.h"

namespace GL
{

class FrameBuffer{
public:
    FrameBuffer(bool depthBuffer = true);
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

    bool HasDepthBuffer() const { return DepthRBO != 0; };
protected:
    GLuint FBO = 0;

    GLuint DepthRBO = 0;

    std::vector<GL::Texture*> attachments;

    glm::uvec2 size = glm::uvec2(800, 600);
};
}