#include "FrameBuffer.h"

#include "Debug/Logger.h"

namespace GL
{
template<FrameBufferType ColorType, FrameBufferType DepthStencilType>
inline FrameBuffer<ColorType, DepthStencilType>::FrameBuffer(uint8_t MSAA_Samples)
    : MSAA_Samples(MSAA_Samples)
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    if constexpr (ColorType == FrameBufferType::RenderBuffer) {
        glGenRenderbuffers(1, &attachmentColor);
        glBindRenderbuffer(GL_RENDERBUFFER, attachmentColor);

        if(MSAA_Samples > 1)
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_Samples, GL_RGB, size.x, size.y);
        else
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, size.x, size.y);
            
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, attachmentColor);
    } else{
        glGenTextures(1, &attachmentColor);
        glBindTexture(GetTextureTarget(), attachmentColor);

        if(GetTextureTarget() == GL_TEXTURE_2D_MULTISAMPLE)
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_Samples, GL_RGB, size.x, size.y, GL_TRUE);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr); 
        
        glTexParameteri(GetTextureTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GetTextureTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GetTextureTarget(), attachmentColor, 0);
    }

    if constexpr (DepthStencilType == FrameBufferType::RenderBuffer) {
        glGenRenderbuffers(1, &attachmentDepthStencil);
        glBindRenderbuffer(GL_RENDERBUFFER, attachmentDepthStencil);

        if(MSAA_Samples > 1)
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_Samples, GL_DEPTH24_STENCIL8, size.x, size.y);
        else
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, attachmentDepthStencil);
    } else{
        glGenTextures(1, &attachmentDepthStencil);
        glBindTexture(GetTextureTarget(), attachmentDepthStencil);

        if(GetTextureTarget() == GL_TEXTURE_2D_MULTISAMPLE)
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_Samples, GL_DEPTH24_STENCIL8, size.x, size.y, GL_TRUE);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, size.x, size.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr); 
        
        glTexParameteri(GetTextureTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GetTextureTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GetTextureTarget(), attachmentDepthStencil, 0);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        Debug::LogError("Failed to create FrameBuffer!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

template<FrameBufferType ColorType, FrameBufferType DepthStencilType>
inline FrameBuffer<ColorType, DepthStencilType>::~FrameBuffer()
{
    if(FBO != 0) glDeleteFramebuffers(1, &FBO);
    
    if(attachmentColor != 0) {
        if constexpr (ColorType == FrameBufferType::RenderBuffer) {
            glDeleteRenderbuffers(1, &attachmentColor);
        } else{
            glDeleteTextures(1, &attachmentColor);
        }
    }
    if(attachmentDepthStencil != 0) {
        if constexpr (DepthStencilType == FrameBufferType::RenderBuffer) {
            glDeleteRenderbuffers(1, &attachmentDepthStencil);
        } else{
            glDeleteTextures(1, &attachmentDepthStencil);
        }
    }
}

/// @brief Binds the framebuffer and clears its attachments
template <FrameBufferType ColorType, FrameBufferType DepthStencilType>
inline void FrameBuffer<ColorType, DepthStencilType>::BindAndClear() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

template <FrameBufferType ColorType, FrameBufferType DepthStencilType>
inline void FrameBuffer<ColorType, DepthStencilType>::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/// @brief Renders the framebuffer on a quad or directly on screen based on ColorType
/// @param quadShader shader to render a screen quad (optional for RenderBuffer color type)
/// @param quadVAO vertex array object for the screen quad (optional for RenderBuffer color type or when using MSAA)
template <FrameBufferType ColorType, FrameBufferType DepthStencilType>
inline void FrameBuffer<ColorType, DepthStencilType>::Render(GL::Shader *quadShader, GL::VertexArray *quadVAO) const
{
    this->Unbind();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    if constexpr (ColorType == FrameBufferType::Texture) {
        if(MSAA_Samples > 1){
            // TODO: MSAA textures dont support post-processing yet because im lazy
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
        }else{
            if(!quadShader || !quadVAO) {
                Debug::LogError("FrameBuffer::Render called with Texture color attachment but quadShader or quadVAO is null!");
                return;
            }

            quadShader->Use();
            quadVAO->Bind();
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, attachmentColor);

            glDrawArrays(GL_TRIANGLES, 0, 6);   
        }
    } else{
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

template <FrameBufferType ColorType, FrameBufferType DepthStencilType>
inline FrameBuffer<ColorType, DepthStencilType>::FrameBuffer(FrameBuffer &&other) noexcept
{
    FBO = other.FBO;
    attachmentColor = other.attachmentColor;
    attachmentDepthStencil = other.attachmentDepthStencil;
    other.FBO = 0;
    other.attachmentColor = 0;
    other.attachmentDepthStencil = 0;
}

template <FrameBufferType ColorType, FrameBufferType DepthStencilType>
inline FrameBuffer<ColorType, DepthStencilType> &FrameBuffer<ColorType, DepthStencilType>::operator=(FrameBuffer &&other) noexcept
{
    if (this != &other) {
        if(FBO != 0) glDeleteFramebuffers(1, &FBO);
        if(attachmentColor != 0) {
            if constexpr (ColorType == FrameBufferType::RenderBuffer) {
                glDeleteRenderbuffers(1, &attachmentColor);
            } else{
                glDeleteTextures(1, &attachmentColor);
            }
        }
        if(attachmentDepthStencil != 0) {
            if constexpr (DepthStencilType == FrameBufferType::RenderBuffer) {
                glDeleteRenderbuffers(1, &attachmentDepthStencil);
            } else{
                glDeleteTextures(1, &attachmentDepthStencil);
            }
        }

        FBO = other.FBO;
        attachmentColor = other.attachmentColor;
        attachmentDepthStencil = other.attachmentDepthStencil;

        other.FBO = 0;
        other.attachmentColor = 0;
        other.attachmentDepthStencil = 0;
    }
    return *this;
}
template <FrameBufferType ColorType, FrameBufferType DepthStencilType>
inline void FrameBuffer<ColorType, DepthStencilType>::UpdateSize(const glm::uvec2 &newSize)
{
    if(this->size == newSize) return;

    this->size = newSize;

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    if constexpr (ColorType == FrameBufferType::RenderBuffer) {
        glBindRenderbuffer(GL_RENDERBUFFER, attachmentColor);

        if(MSAA_Samples > 1)
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_Samples, GL_RGB, size.x, size.y);
        else
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, size.x, size.y);
    } else{
        glBindTexture(GetTextureTarget(), attachmentColor);

        if(MSAA_Samples > 1)
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_Samples, GL_RGB, size.x, size.y, GL_TRUE);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr); 
    }

    if constexpr (DepthStencilType == FrameBufferType::RenderBuffer) {
        glBindRenderbuffer(GL_RENDERBUFFER, attachmentDepthStencil);

        if(MSAA_Samples > 1)
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_Samples, GL_DEPTH24_STENCIL8, size.x, size.y);
        else
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    } else{
        glBindTexture(GetTextureTarget(), attachmentDepthStencil);

        if(MSAA_Samples > 1)
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_Samples, GL_DEPTH24_STENCIL8, size.x, size.y, GL_TRUE);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, size.x, size.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr); 
    }
}
}
