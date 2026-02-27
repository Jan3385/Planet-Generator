#include "FrameBuffer.h"

#include <format>

#include "Debug/Logger.h"

namespace GL
{
FrameBuffer::FrameBuffer()
    : FBO(0), depthBufferType(DepthBufferMode::None), depthStorage(0)
{
    
}

FrameBuffer::FrameBuffer(DepthBufferMode mode)
    : depthBufferType(mode)
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // generate depth buffer
    if(mode == DepthBufferMode::RenderBuffer) {
        glGenRenderbuffers(1, &depthStorage);
        glBindRenderbuffer(GL_RENDERBUFFER, depthStorage);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthStorage);
    }else if(mode == DepthBufferMode::Texture){
        glGenTextures(1, &depthStorage);
        glBindTexture(GL_TEXTURE_2D, depthStorage);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        glBindTexture(GL_TEXTURE_2D, 0);
    } else{
        this->depthStorage = 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    if(this->FBO == 0) [[unlikely]] {
        Debug::LogError("Trying to add texture to uninitialized framebuffer!");
        return;
    }

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
    if(this->FBO == 0) [[unlikely]] {
        Debug::LogError("Trying to complete setup of uninitialized framebuffer!");
        return;
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Debug::LogError("Failed to complete framebuffer setup!");
    }
}

/// @brief Binds the framebuffer with textures and clears its attachments
void FrameBuffer::BindShaderFBO() const
{
    if(this->FBO == 0) [[unlikely]] {
        Debug::LogError("Trying to bind uninitialized framebuffer!");
        return;
    }

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
    if(this->FBO == 0) [[unlikely]] {
        Debug::LogError("Trying to bind textures of uninitialized framebuffer!");
        return;
    }

    for(size_t i = 0; i < attachments.size(); i++) {
        attachments[i]->BindToUnit(static_cast<uint8_t>(i));
    }
}

void FrameBuffer::BindTextures(uint8_t start) const
{
    if(this->FBO == 0) [[unlikely]] {
        Debug::LogError("Trying to bind textures of uninitialized framebuffer!");
        return;
    }

    for(size_t i = 0; i < attachments.size(); i++) {
        attachments[i]->BindToUnit(static_cast<uint8_t>(start + i));
    }
}

void FrameBuffer::BindTextureTo(uint8_t attachmentIndex, uint8_t unit) const
{
    if(this->FBO == 0) [[unlikely]] {
        Debug::LogError("Trying to bind texture of uninitialized framebuffer!");
        return;
    }

    if(attachmentIndex >= attachments.size()) {
        Debug::LogError(std::format("Attachment index {0} is out of bounds for FrameBuffer with {1} attachments!", attachmentIndex, attachments.size()));
        return;
    }
    
    attachments[attachmentIndex]->BindToUnit(unit);
}

void FrameBuffer::UnbindShaderFBO() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::CopyDepthToFBO(GLuint targetFBO) const
{
    if(this->FBO == 0) [[unlikely]] {
        Debug::LogError("Trying to copy depth from uninitialized framebuffer!");
        return;
    }
    if(targetFBO == 0) [[unlikely]] {
        Debug::LogError("Trying to copy depth to uninitialized framebuffer!");
        return;
    }
    if(depthStorage == 0) [[unlikely]] {
        Debug::LogError("Trying to copy depth from framebuffer without depth buffer!");
        return;
    }

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
    if(this->FBO == 0) [[unlikely]] {
        Debug::LogError("Trying to update size of uninitialized framebuffer!");
        return;
    }

    if(this->size == newSize) return;

    this->size = newSize;

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    for(size_t i = 0; i < attachments.size(); i++) {
        attachments[i]->Bind();
        attachments[i]->Resize(size.x, size.y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, attachments[i]->GetID(), 0);
    }

    if(depthStorage != 0) {
        glBindRenderbuffer(GL_RENDERBUFFER, depthStorage);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthStorage);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
}
