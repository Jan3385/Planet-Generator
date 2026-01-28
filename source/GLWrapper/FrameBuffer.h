#pragma once

#include <glad/glad.h>

#include "GLWrapper/Shader.h"
#include "GLWrapper/VertexArray.h"

namespace GL
{
enum class FrameBufferColorType{
    RenderBuffer,
    Texture
};
enum class FrameBufferDepthStencilType{
    None,
    Texture
};

template<FrameBufferColorType ColorType, FrameBufferDepthStencilType DepthStencilType>
class FrameBuffer{
public:
    FrameBuffer(uint8_t MSAA_Samples = 0);
    ~FrameBuffer();

    void BindAndClear() const;
    void Unbind() const;
    void Render(GL::Shader *quadShader = nullptr, GL::VertexArray *quadVAO = nullptr) const;

    // disable copy semantics
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    // move semantics
    FrameBuffer(FrameBuffer&&other) noexcept;
    FrameBuffer& operator=(FrameBuffer&&other) noexcept;

    void UpdateSize(const glm::uvec2& newSize);

    glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
protected:
    GLuint FBO = 0;

    glm::uvec2 size = glm::uvec2(800, 600);
    uint8_t MSAA_Samples = 0;

    GLuint rboColor = 0;
    GLuint rboDepthStencil = 0;

    // Used for multisampled texture ColorType
    GLuint postFBO = 0;
    GLuint renderedTexture = 0;
    GLuint renderedDepthStencilTexture = 0;
};
}

#include "FrameBuffer.inl"