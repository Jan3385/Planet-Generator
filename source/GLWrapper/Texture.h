#pragma once

#include <glad/glad.h>
#include <string>

namespace GL
{
enum class TextureWrapMode{
    Repeat = GL_REPEAT,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER,
    MirrorClampToEdge = GL_MIRROR_CLAMP_TO_EDGE,
};
enum class MipmapMode{
    None,
    Nearest,
    Linear
};
enum class TextureFormat{
    RED = GL_RED,
    RED_GREEN = GL_RG,
    RGB = GL_RGB,
    BGR = GL_BGR,
    RGBA = GL_RGBA,
    BGRA = GL_BGRA,
    RED_INT = GL_RED_INTEGER,
    RED_GREEN_INT = GL_RG_INTEGER,
    RGB_INT = GL_RGB_INTEGER,
    BGR_INT = GL_BGR_INTEGER,
    RGBA_INT = GL_BGR_INTEGER,
    BGRA_INT = GL_BGRA_INTEGER,
    STENCIL_INDEX = GL_STENCIL_INDEX,
    DEPTH = GL_DEPTH_COMPONENT,
    DEPTH_STENCIL = GL_DEPTH_STENCIL
};

class Texture{
public:
    Texture();
    Texture(TextureWrapMode wrap, MipmapMode mipmap, bool blurred);
    ~Texture();

    // disable copy semantics
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // move semantics
    Texture(Texture&&other) noexcept;
    Texture& operator=(Texture&&other) noexcept;

    void GenTexture(TextureFormat format, GLuint internalFormat, int width, int height);
    void GenTexture(TextureFormat format, GLuint internalFormat, std::string filePath, bool flip = true);
    void GenTexture(TextureFormat format, GLuint internalFormat, unsigned char *data, int width, int height);

    void Bind();
    static void Unbind() { glBindTexture(GL_TEXTURE_2D, 0); };
    void BindToUnit(uint8_t unit);
    
    bool hasMipmaps() { return this->mipmapsGenerated; };
    bool isBlurred()  { return this->blurred; };
    TextureWrapMode wrapMode() { return this->currentWrapMode; };
protected:
    GLuint ID = 0;
    static GLuint GetTextureFiltering(MipmapMode mipmap, bool blurred);
private:
    bool mipmapsGenerated;
    bool blurred;
    TextureWrapMode currentWrapMode;

    bool textureGenerated = false;
};

}