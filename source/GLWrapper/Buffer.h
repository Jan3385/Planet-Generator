#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace GL{
enum BufferTarget : GLenum {
    ArrayBuffer = GL_ARRAY_BUFFER,
    ElementArrayBuffer = GL_ELEMENT_ARRAY_BUFFER,
    ShaderStorageBuffer = GL_SHADER_STORAGE_BUFFER,
    UniformBuffer = GL_UNIFORM_BUFFER,
    CopyReadBuffer = GL_COPY_READ_BUFFER,
    CopyWriteBuffer = GL_COPY_WRITE_BUFFER
};

/// @brief Base class for OpenGL buffer objects
class BufferBase {
public:
    virtual ~BufferBase() = default;

    virtual bool IsInitialized() const { return ID != 0; }

    virtual void Bind() const = 0;
    static void Unbind() { };
protected:
    GLuint ID;
    std::string name;
};

/// @brief OpenGL buffer object wrapper
/// @tparam T Data type stored in the buffer
/// @tparam Target OpenGL buffer target (e.g., GL_ARRAY_BUFFER)
template<typename T, GLenum Target>
class Buffer : public BufferBase {
public:
    Buffer();
    Buffer(std::string name);
    ~Buffer();

    // Disable copy
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    // Movable
    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    void Bind() const;
    static void Unbind();

    void SetData(const T& data, GLenum usage);
    void SetData(const T* data, GLuint size, GLenum usage);
    void SetData(const std::vector<T>& data, GLenum usage);
    void UpdateData(GLuint offset, const std::vector<T>& data) const;
    void UpdateData(GLuint offset, const T* data, GLuint size) const;

    template <BufferTarget otherTarget>
    void UploadBufferIn(GLuint copyOffset, GLuint writeOffset, Buffer<T, otherTarget>& buffer, GLuint size) const;

    void BindBufferBase(GLuint binding) const;

    void ClearBuffer();
    void ClearBuffer(const GLuint newSize, GLenum usage);

    void SetSize(GLuint size, bool silenceWarnings = false);
    GLuint GetSize() const { return bufferSize; };

    T* ReadBuffer() const;
    T* ReadBuffer(GLuint size) const;

    template<typename, GLenum>
    friend class Buffer;

    template<typename>
    friend class GLGroupStorageBuffer;

private:
    GLint bufferSize = 0;
};
}

#include "Buffer.inl"