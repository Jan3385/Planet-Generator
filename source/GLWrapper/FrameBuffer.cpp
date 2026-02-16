#include "FrameBuffer.h"

#include <format>

#include "Debug/Logger.h"

namespace GL
{
FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    //TODO: temp depth RBO
    glGenRenderbuffers(1, &DepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, DepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRBO);
}

FrameBuffer::~FrameBuffer()
{
    if(FBO != 0) glDeleteFramebuffers(1, &FBO);

    for(auto t : attachments) {
        delete t;
    }
}

void FrameBuffer::AddBufferTexture(GLenum internalFormat, GL::TextureFormat format, GLenum type)
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    Texture* t = new GL::Texture(
        GL::TextureWrapMode::ClampToEdge,
        GL::MipmapMode::None,
        false
    );
    t->GenTexture(
        format, internalFormat, type, size.x, size.y
    );

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachments.size(), GL_TEXTURE_2D, t->GetID(), 0);

    attachments.push_back(t);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::CompleteSetup()
{
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Debug::LogError("Failed to complete framebuffer setup!");
    }
}

/// @brief Binds the framebuffer with textures and clears its attachments
void FrameBuffer::BindShaderFBO() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    if(attachments.size() > 31) [[unlikely]] {
        Debug::LogError(std::format("Too many attachments for FrameBuffer! Max is 32, current is {0}", attachments.size()));
    }

    uint8_t numAttachments = static_cast<uint8_t>(attachments.size());
    GLuint *drawBuffers = new GLuint[numAttachments];
    for(uint8_t i = 0; i < numAttachments; i++) {
        drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    glDrawBuffers(static_cast<GLsizei>(numAttachments), drawBuffers);
    delete[] drawBuffers;
}

void FrameBuffer::BindTextures() const
{
    for(size_t i = 0; i < attachments.size(); i++) {
        attachments[i]->BindToUnit(static_cast<uint8_t>(i));
    }
}

void FrameBuffer::BindTextures(uint8_t start) const
{
    for(size_t i = 0; i < attachments.size(); i++) {
        attachments[i]->BindToUnit(static_cast<uint8_t>(start + i));
    }
}

void FrameBuffer::UnbindShaderFBO() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::CopyDepthToFBO(GLuint targetFBO) const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO);
    glBlitFramebuffer(
        0, 0, size.x, size.y,
        0, 0, size.x, size.y,
        GL_DEPTH_BUFFER_BIT,
        GL_NEAREST
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::CopyDepthToFBO(FrameBuffer &targetFBO) const
{
    this->CopyDepthToFBO(targetFBO.FBO);
}

FrameBuffer::FrameBuffer(FrameBuffer &&other) noexcept
{
    this->FBO = other.FBO;
    this->attachments = std::move(other.attachments);
    this->size = other.size;

    other.FBO = 0;
}

FrameBuffer &FrameBuffer::operator=(FrameBuffer &&other) noexcept
{
    if (this != &other) {
        if(FBO != 0) glDeleteFramebuffers(1, &FBO);

        this->attachments = std::move(other.attachments);
        this->size = other.size;
        FBO = other.FBO;
        
        other.FBO = 0;
    }
    return *this;
}

/// @brief Updates the size of the framebuffer and its attachments
/// @param newSize New size of the framebuffer in pixels
void FrameBuffer::UpdateSize(const glm::uvec2 &newSize)
{
    if(this->size == newSize) return;

    this->size = newSize;

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    for(size_t i = 0; i < attachments.size(); i++) {
        attachments[i]->Bind();
        attachments[i]->Resize(size.x, size.y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, attachments[i]->GetID(), 0);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, DepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
}
