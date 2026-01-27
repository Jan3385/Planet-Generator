#pragma once

#include <glad/glad.h>

#include "GLWrapper/Shader.h"
#include "GLWrapper/VertexArray.h"

namespace GL
{
enum class FrameBufferType{
    RenderBuffer,
    Texture
};

template<FrameBufferType ColorType, FrameBufferType DepthStencilType>
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

    glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
protected:
    GLuint FBO = 0;

    GLuint attachmentColor = 0;
    GLuint attachmentDepthStencil = 0;
};
}

#include "FrameBuffer.inl"