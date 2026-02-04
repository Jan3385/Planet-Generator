#include "FrameBuffer.h"

#include "Debug/Logger.h"

namespace GL
{
template<FrameBufferColorType ColorType, FrameBufferDepthStencilType DepthStencilType>
inline FrameBuffer<ColorType, DepthStencilType>::FrameBuffer(bool clamped, uint8_t MSAA_Samples)
    : MSAA_Samples(MSAA_Samples), clamped(clamped)
{
    // make FBO and RBOs
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenRenderbuffers(1, &rboColor);
    glGenRenderbuffers(1, &rboDepthStencil);

    // Color RBO
    glBindRenderbuffer(GL_RENDERBUFFER, rboColor);

    GLuint colorFromat = clamped ? GL_RGB16F : GL_RGB;

    if(MSAA_Samples > 1)
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_Samples, colorFromat, size.x, size.y);
    else
        glRenderbufferStorage(GL_RENDERBUFFER, colorFromat, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboColor);

    // DepthStencil RBO
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    if(MSAA_Samples > 1)
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_Samples, GL_DEPTH24_STENCIL8, size.x, size.y);
    else
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);

    if constexpr (ColorType == FrameBufferColorType::Texture){
        glGenFramebuffers(1, &postFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, postFBO);

        glGenTextures(1, &renderedTexture);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, colorFromat, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

        if constexpr (DepthStencilType == FrameBufferDepthStencilType::Texture){
            glGenTextures(1, &renderedDepthStencilTexture);
            glBindTexture(GL_TEXTURE_2D, renderedDepthStencilTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, size.x, size.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, renderedDepthStencilTexture, 0);
        }
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        Debug::LogError("Failed to create FrameBuffer!");
    }

    if constexpr (ColorType == FrameBufferColorType::Texture){
        glBindFramebuffer(GL_FRAMEBUFFER, postFBO);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
            Debug::LogError("Failed to create Post FrameBuffer!");
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

template<FrameBufferColorType ColorType, FrameBufferDepthStencilType DepthStencilType>
inline FrameBuffer<ColorType, DepthStencilType>::~FrameBuffer()
{
    if(FBO != 0) glDeleteFramebuffers(1, &FBO);
    if(postFBO != 0) glDeleteFramebuffers(1, &postFBO);
    
    if(rboColor != 0) glDeleteRenderbuffers(1, &rboColor);
    if(rboDepthStencil != 0) glDeleteRenderbuffers(1, &rboDepthStencil);
    if(renderedTexture != 0) glDeleteTextures(1, &renderedTexture);
    if(renderedDepthStencilTexture != 0) glDeleteTextures(1, &renderedDepthStencilTexture);
}

/// @brief Binds the framebuffer and clears its attachments
template <FrameBufferColorType ColorType, FrameBufferDepthStencilType DepthStencilType>
inline void FrameBuffer<ColorType, DepthStencilType>::BindAndClear() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

template <FrameBufferColorType ColorType, FrameBufferDepthStencilType DepthStencilType>
inline void FrameBuffer<ColorType, DepthStencilType>::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/// @brief Renders the framebuffer on a quad or directly on screen based on ColorType
/// @param quadShader shader to render a screen quad (optional for RenderBuffer color type)
/// @param quadVAO vertex array object for the screen quad (optional for RenderBuffer color type)
template <FrameBufferColorType ColorType, FrameBufferDepthStencilType DepthStencilType>
inline void FrameBuffer<ColorType, DepthStencilType>::Render(GL::Shader *quadShader, GL::VertexArray *quadVAO) const
{
    this->Unbind();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    if constexpr (ColorType == FrameBufferColorType::Texture) {
        // Render to screen quad

        if(!quadShader || !quadVAO) {
            Debug::LogError("FrameBuffer::Render called with multisampled Texture color attachment but quadShader or quadVAO is null!");
            return;
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postFBO);

        GLuint blitMask = GL_COLOR_BUFFER_BIT;
        if constexpr (DepthStencilType == FrameBufferDepthStencilType::Texture){
            blitMask |= GL_DEPTH_BUFFER_BIT;
        }
        glBlitFramebuffer(
            0, 0, 
            size.x, size.y, 
            0, 0, 
            size.x, size.y, 
            blitMask, GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        quadShader->Use();
        quadVAO->Bind();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);

        if constexpr (DepthStencilType == FrameBufferDepthStencilType::Texture){
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, renderedDepthStencilTexture);
        }

        glDrawArrays(GL_TRIANGLES, 0, 6);
    } else{
        // Render directly to screen

        glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitFramebuffer(
            0, 0, 
            size.x, size.y, 
            0, 0, 
            size.x, size.y, 
            GL_COLOR_BUFFER_BIT, GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

template <FrameBufferColorType ColorType, FrameBufferDepthStencilType DepthStencilType>
inline FrameBuffer<ColorType, DepthStencilType>::FrameBuffer(FrameBuffer &&other) noexcept
{
    FBO = other.FBO;
    rboColor = other.rboColor;
    rboDepthStencil = other.rboDepthStencil;
    postFBO = other.postFBO;
    renderedTexture = other.renderedTexture;
    renderedDepthStencilTexture = other.renderedDepthStencilTexture;
    other.FBO = 0;
    other.rboColor = 0;
    other.rboDepthStencil = 0;
    other.postFBO = 0;
    other.renderedTexture = 0;
    other.renderedDepthStencilTexture = 0;
}

template <FrameBufferColorType ColorType, FrameBufferDepthStencilType DepthStencilType>
inline FrameBuffer<ColorType, DepthStencilType> &FrameBuffer<ColorType, DepthStencilType>::operator=(FrameBuffer &&other) noexcept
{
    if (this != &other) {
        if(FBO != 0) glDeleteFramebuffers(1, &FBO);
        if(postFBO != 0) glDeleteFramebuffers(1, &postFBO);
        if(rboColor != 0) glDeleteRenderbuffers(1, &rboColor);
        if(rboDepthStencil != 0) glDeleteRenderbuffers(1, &rboDepthStencil);
        if(renderedTexture != 0) glDeleteTextures(1, &renderedTexture);
        if(renderedDepthStencilTexture != 0) glDeleteTextures(1, &renderedDepthStencilTexture);
        FBO = other.FBO;
        rboColor = other.rboColor;
        rboDepthStencil = other.rboDepthStencil;
        postFBO = other.postFBO;
        renderedTexture = other.renderedTexture;
        renderedDepthStencilTexture = other.renderedDepthStencilTexture;
        other.FBO = 0;
        other.rboColor = 0;
        other.rboDepthStencil = 0;
        other.postFBO = 0;
        other.renderedTexture = 0;
        other.renderedDepthStencilTexture = 0;
    }
    return *this;
}

/// @brief Updates the size of the framebuffer and its attachments
/// @param newSize New size of the framebuffer in pixels
template <FrameBufferColorType ColorType, FrameBufferDepthStencilType DepthStencilType>
inline void FrameBuffer<ColorType, DepthStencilType>::UpdateSize(const glm::uvec2 &newSize)
{
    if(this->size == newSize) return;

    this->size = newSize;

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Update Color RBO
    glBindRenderbuffer(GL_RENDERBUFFER, rboColor);
    if(MSAA_Samples > 1)
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_Samples, GL_RGB, size.x, size.y);
    else
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, size.x, size.y);
    
    // Update DepthStencil RBO
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    if(MSAA_Samples > 1)
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_Samples, GL_DEPTH24_STENCIL8, size.x, size.y);
    else
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);

    // Update Texture attachments
    if constexpr (ColorType == FrameBufferColorType::Texture){
        glBindFramebuffer(GL_FRAMEBUFFER, postFBO);

        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        if constexpr (DepthStencilType == FrameBufferDepthStencilType::Texture){
            glBindTexture(GL_TEXTURE_2D, renderedDepthStencilTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, size.x, size.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
}
