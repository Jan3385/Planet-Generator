namespace GL {
    /// @brief Adds a *FLOAT* vertex attribute to the vertex array
    /// @tparam T The data added to the vertex array
    /// @param layoutIndex The index of the layout location
    /// @param size The number of components per vertex attribute
    /// @param buffer The buffer object containing the vertex data
    /// @param normalized Whether the vertex attribute is normalized meaning [0, 1] or [-1, 1] range for floats (based on GLType)
    /// @param offset The offset of the attribute data within the buffer (use `offsetof()` )
    /// @param divisor The divisor for instanced rendering (0 = 1 per vertex, n = n per instance)
    template<typename AttribType, typename T>
    void VertexArray::AddAttribute(GLuint layoutIndex, GLint size, const Buffer<T, GL::BufferTarget::ArrayBuffer>& buffer, 
        GLboolean normalized, size_t offset, GLuint divisor) const {
        this->Bind();

        buffer.Bind();
        glEnableVertexAttribArray(layoutIndex);

        size_t stride = std::is_class<T>::value ? sizeof(T) : 0;

        glVertexAttribPointer(layoutIndex, size, GLType<AttribType>::value, normalized, stride, (void*)offset);

        if (divisor > 0)
            glVertexAttribDivisor(layoutIndex, divisor);
    }

    /// @brief Adds an *INTEGER* vertex attribute to the vertex array
    /// @tparam T The data added to the vertex array
    /// @param layoutIndex The index of the layout location
    /// @param size The number of components per vertex attribute
    /// @param buffer The buffer object containing the vertex data
    /// @param offset The offset of the attribute data within the buffer (use `offsetof()` )
    /// @param divisor The divisor for instanced rendering (0 = 1 per vertex, n = n per instance)
    template<typename AttribType, typename T>
    void VertexArray::AddIntAttribute(GLuint layoutIndex, GLint size, const Buffer<T, GL::BufferTarget::ArrayBuffer>& buffer, 
        size_t offset, GLuint divisor) const {
        this->Bind();

        buffer.Bind();
        glEnableVertexAttribArray(layoutIndex);

        size_t stride = std::is_class<T>::value ? sizeof(T) : 0;

        glVertexAttribIPointer(layoutIndex, size, GLType<AttribType>::value, stride, (void*)offset);

        if (divisor > 0)
            glVertexAttribDivisor(layoutIndex, divisor);
    }
}