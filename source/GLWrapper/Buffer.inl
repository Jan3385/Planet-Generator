#include <iostream>
#include <cstring>
#include "Buffer.h"
#include "Debug/Logger.h"

namespace GL{

template<typename T, GLenum Target>
Buffer<T, Target>::Buffer() {
    this->ID = 0;
}

template <typename T, GLenum Target>
inline Buffer<T, Target>::Buffer(std::string name)
{
    glGenBuffers(1, &ID);
    this->name = name;
    this->Bind();
    glObjectLabel(GL_BUFFER, ID, -1, this->name.c_str());
    this->Unbind();
}

template <typename T, GLenum Target>
inline Buffer<T, Target>::~Buffer()
{
    if(ID != 0)
        glDeleteBuffers(1, &ID);
    ID = 0;
}

template <typename T, GLenum Target>
inline Buffer<T, Target>::Buffer(Buffer &&other) noexcept
    : bufferSize(other.bufferSize)
{
    this->name = std::move(other.name);
    this->ID = other.ID;
    other.ID = 0;
    other.bufferSize = 0;
    other.name.clear();
}

template <typename T, GLenum Target>
inline Buffer<T, Target> &Buffer<T, Target>::operator=(Buffer<T, Target> &&other) noexcept
{
    if (this != &other) {
        glDeleteBuffers(1, &ID);
        ID = other.ID;
        bufferSize = other.bufferSize;
        name = std::move(other.name);
        other.ID = 0;
        other.bufferSize = 0;
        other.name.clear();
    }
    return *this;
}

template <typename T, GLenum Target>
inline void Buffer<T, Target>::Bind() const
{
    if(ID == 0)
        Debug::LogFatal("Attempt to bind uninitialized Buffer: " + this->name);

    glBindBuffer(Target, ID);
}

template <typename T, GLenum Target>
inline void Buffer<T, Target>::Unbind()
{
    glBindBuffer(Target, 0);
}

/// @brief Sets a variable for the buffer object (Causes reallocation)
/// @param data Variable to set
/// @param usage GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW,..
template <typename T, GLenum Target>
inline void Buffer<T, Target>::SetData(const T &data, GLenum usage)
{
    this->Bind();
    BufferData(Target, sizeof(T), &data, usage);
    this->bufferSize = 1;
}

/// @brief Sets a data array (Causes reallocation)
/// @param data Array to set
/// @param size Size of the array
/// @param usage GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW,..
template <typename T, GLenum Target>
inline void Buffer<T, Target>::SetData(const T *data, GLuint size, GLenum usage)
{
    this->Bind();
    glBufferData(Target, size * sizeof(T), data, usage);
    this->bufferSize = size;
    this->Unbind();
}

/// @brief Sets a data vector (Causes reallocation)
/// @param data Vector to set
/// @param usage GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW,..
template <typename T, GLenum Target>
inline void Buffer<T, Target>::SetData(const std::vector<T> &data, GLenum usage)
{
    this->Bind();
    if constexpr (!std::is_same_v<T, bool>) {
        BufferData(Target, data.size() * sizeof(T), data.data(), usage);
        this->bufferSize = static_cast<GLint>(data.size());
    } else{
        // special behaviour for std::vector<bool>
        std::vector<uint32_t> boolData(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            boolData[i] = data[i] ? 1 : 0;
        }

        glBufferData(Target, boolData.size() * sizeof(uint32_t), boolData.data(), usage);
        this->bufferSize = static_cast<GLint>(data.size());
    }
}

/// @brief Updates a portion of the buffer with new data
/// @param offset Offset in the buffer to update
/// @param data New data vector to update
template <typename T, GLenum Target>
inline void Buffer<T, Target>::UpdateData(GLuint offset, const std::vector<T> &data) const
{
    if (offset + data.size() > this->bufferSize) {
        Debug::LogError("[" + this->name + "] Buffer::UpdateData VEC - Error: Attempt to update buffer data out of bounds!");
        return;
    }

    this->Bind();
    BufferSubData(Target, offset * sizeof(T), data.size() * sizeof(T), data.data());
}

/// @brief Updates a portion of the buffer with new data
/// @param offset Offset in the buffer to update
/// @param data New data array to update
/// @param size Size of the array
template <typename T, GLenum Target>
inline void Buffer<T, Target>::UpdateData(GLuint offset, const T *data, GLuint size) const
{
    if (offset + size > static_cast<GLuint>(this->bufferSize)) {
        Debug::LogError("[" + this->name + "] Buffer::UpdateData ARR - Error: Attempt to update buffer data out of bounds!");
        Debug::LogError("Offset: " + std::to_string(offset) + ", Size: " + std::to_string(size) + ", BufferSize: " + std::to_string(this->bufferSize));
        return;
    }

    this->Bind();
    BufferSubData(Target, offset * sizeof(T), size * sizeof(T), data);
}

/// @brief Uploads a portion of the buffer from another buffer
/// @param copyOffset Offset in the source buffer to copy from (`buffer`)
/// @param writeOffset Offset in the destination buffer to write to (`this`)
/// @param buffer Source buffer to copy from
/// @param size Size of the data to copy (if 0, copy entire `buffer`)
template <typename T, GLenum Target>
template <BufferTarget otherTarget>
inline void Buffer<T, Target>::UploadBufferIn(GLuint copyOffset, GLuint writeOffset, Buffer<T, otherTarget>& buffer, GLuint size) const
{
    if(size == 0){
        size = buffer.bufferSize;
    }

    if (copyOffset + size > static_cast<GLuint>(buffer.bufferSize)) {
        Debug::LogError("[" + this->name + "] Buffer::UploadBufferIn - Error: Attempt to upload buffer data out of bounds!(source)");
        copyOffset = buffer.bufferSize - size;
    }
    if (writeOffset + size > static_cast<GLuint>(this->bufferSize)) {
        Debug::LogError("[" + this->name + "] Buffer::UploadBufferIn - Error: Attempt to upload buffer data out of bounds!(destination)");
    }

    glBindBuffer(GL_COPY_READ_BUFFER, buffer.ID);
    glBindBuffer(GL_COPY_WRITE_BUFFER, this->ID);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, copyOffset * sizeof(T), writeOffset * sizeof(T), size * sizeof(T));
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

/// @brief Binds the buffer to a specific binding point
/// @param binding Binding point to use
template <typename T, GLenum Target>
inline void Buffer<T, Target>::BindBufferBase(GLuint binding) const
{
    if(ID == 0){
        Debug::LogFatal("Attempt to bind base uninitialized Buffer: " + this->name);
    }

    glBindBufferBase(Target, binding, ID);
}

/// @brief Clears the buffer data (sets to zero)
template <typename T, GLenum Target>
inline void Buffer<T, Target>::ClearBuffer()
{
    if(ID == 0){
        Debug::LogFatal("Attempt to clear uninitialized Buffer: " + this->name);
    }
    this->Bind();

    #ifdef GL_VERSION_4_3
    if constexpr (std::is_same_v<T, float>) {
        glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32F, GL_RED, GL_FLOAT, nullptr);
    }else{
        void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
        if (ptr) {
            memset(ptr, 0, this->bufferSize * sizeof(T));
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }
    }
    #else
    // Fallback - map and memset to zero
    this->Bind();
    void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    if (ptr) {
        memset(ptr, 0, size * sizeof(T));
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
    #endif
}

/// @brief Clears AND resizes (if needed) the buffer
template <typename T, GLenum Target>
inline void Buffer<T, Target>::ClearBuffer(const GLuint newSize, GLenum usage)
{
    this->Bind();
    if(this->bufferSize == static_cast<GLint>(newSize))
        this->ClearBuffer();
    else{
        std::vector<T> emptyData(newSize);
        this->SetData(emptyData, usage);
    }
}
/// @brief Sets the size variable inside the object
/// @warning Does not reallocate the buffer. ! Only use this when you know what you are doing !
/// @param size New size (in number of elements) of the buffer
/// @param silenceWarnings suppresses warnings at undefined or dangerous behaviour
template <typename T, GLenum Target>
inline void Buffer<T, Target>::SetSize(GLuint size, bool silenceWarnings)
{
    if(!silenceWarnings) {
        if (size == 0)
            Debug::LogWarn("[" + this->name + "] Buffer::SetSize - Warning: Setting buffer size to 0!");
        
        if(size > static_cast<GLuint>(this->bufferSize))
            Debug::LogWarn("[" + this->name + "] Buffer::SetSize - Warning: New size is larger than current buffer size! Without reallocation this may lead to undefined behaviour!");
    }

    this->bufferSize = size;
}

/// @brief Reads the buffer data
/// @warning Pointer needs to be deleted after use (Memory leak!)
/// @return Pointer to the buffer data
template <typename T, GLenum Target>
inline T *Buffer<T, Target>::ReadBuffer() const
{
    this->Bind();

    T* data = static_cast<T*>(glMapBuffer(Target, GL_READ_ONLY));
    if (!data) {
        Debug::LogError("[" + this->name + "] Buffer::ReadBuffer - Error: Failed to map buffer to client address space for reading!");
        return nullptr;
    }

    T* result = new T[this->bufferSize];
    std::memcpy(result, data, this->bufferSize * sizeof(T));
    glUnmapBuffer(Target);
    return result;
}

/// @brief Reads the buffer data until the specified size
/// @warning Pointer needs to be deleted after use (Memory leak!)
/// @note If size > bufferSize, then size gets clamped to the bufferSize and prints an error
/// @return Pointer to the buffer data
template <typename T, GLenum Target>
inline T *Buffer<T, Target>::ReadBuffer(GLuint size) const
{
    if(this->bufferSize < static_cast<GLint>(size)){
        Debug::LogWarn("[" + this->name + "] Buffer::ReadBuffer - Warning: Requested size is larger than current buffer size! Returning only available data.");
        size = this->bufferSize;
    }

    this->Bind();

    T* data = static_cast<T*>(glMapBuffer(Target, GL_READ_ONLY));
    if (!data) {
        Debug::LogError("[" + this->name + "] Buffer::ReadBuffer - Error: Failed to map buffer to client address space for reading!");
        return nullptr;
    }

    T* result = new T[size];
    std::memcpy(result, data, size * sizeof(T));
    glUnmapBuffer(Target);
    return result;
}
}