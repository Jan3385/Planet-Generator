#pragma once

#include <glad/glad.h>

#include "GLWrapper/Shader.h"
#include "GLWrapper/VertexArray.h"
#include "GLWrapper/Texture.h"

namespace GL
{

class FrameBuffer{
public:
    FrameBuffer();
    ~FrameBuffer();

    void AddBufferTexture(GLenum internalFormat, GL::TextureFormat format, GLenum type);
    void CompleteSetup();

    void BindShaderFBO() const;
    void BindTextures() const;
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
protected:
    GLuint FBO = 0;

    GLuint DepthRBO = 0;

    std::vector<GL::Texture*> attachments;

    glm::uvec2 size = glm::uvec2(800, 600);
};
}