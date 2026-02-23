#include "Texture.h"

#include "Debug/Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

std::unordered_map<std::string, GLuint> GL::Texture::loadedTextures = std::unordered_map<std::string, GLuint>();

GL::Texture::Texture() : ID(0) { }

GL::Texture::Texture(TextureWrapMode wrap, MipmapMode mipmap, bool blurred)
    : generateMipmaps(mipmap != MipmapMode::None), blurred(blurred), currentWrapMode(wrap)
{
    glGenTextures(1, &ID);
    this->Bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLuint>(wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLuint>(wrap));

    GLuint filterMode = Texture::GetTextureFiltering(mipmap, blurred);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
}

GL::Texture::~Texture()
{
    if(ID != 0) glDeleteTextures(1, &ID);
}

GL::Texture::Texture(const Texture &other)
{
    this->ID = other.ID;
    this->blurred = other.blurred;
    this->generateMipmaps = other.generateMipmaps;
    this->currentWrapMode = other.currentWrapMode;
    this->textureGenerated = other.textureGenerated;
    this->format = other.format;
    this->internalFormat = other.internalFormat;
    this->type = other.type;
}

GL::Texture &GL::Texture::operator=(const Texture &other)
{
    if (this != &other) {
        if(this->ID != 0) glDeleteTextures(1, &this->ID);
        this->ID = other.ID;
        this->blurred = other.blurred;
        this->generateMipmaps = other.generateMipmaps;
        this->currentWrapMode = other.currentWrapMode;
        this->textureGenerated = other.textureGenerated;
        this->format = other.format;
        this->internalFormat = other.internalFormat;
        this->type = other.type;
    }
    return *this;
}

GL::Texture::Texture(GL::Texture &&other) noexcept
{
    this->ID = other.ID;
    this->blurred = other.blurred;
    this->generateMipmaps = other.generateMipmaps;
    this->currentWrapMode = other.currentWrapMode;
    this->textureGenerated = other.textureGenerated;
    this->format = other.format;
    this->internalFormat = other.internalFormat;
    this->type = other.type;
    other.ID = 0;
}

GL::Texture &GL::Texture::operator=(GL::Texture &&other) noexcept
{
    if (this != &other) {
        if(this->ID != 0) glDeleteTextures(1, &this->ID);
        this->ID = other.ID;
        this->blurred = other.blurred;
        this->generateMipmaps = other.generateMipmaps;
        this->currentWrapMode = other.currentWrapMode;
        this->textureGenerated = other.textureGenerated;
        this->format = other.format;
        this->internalFormat = other.internalFormat;
        this->type = other.type;
        other.ID = 0;
    }
    return *this;
}

/// @brief Generates an empty texture
/// @param format format of the texture
/// @param internalFormat internal format of the texture
/// @param width width of the empty texture
/// @param height height of the empty texture
void GL::Texture::GenTexture(TextureFormat format, GLuint internalFormat, GLenum type, int width, int height)
{
    this->Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, static_cast<GLuint>(format), type, nullptr);
    if(this->generateMipmaps) glGenerateMipmap(GL_TEXTURE_2D);
    
    this->textureGenerated = true;

    this->format = format;
    this->internalFormat = internalFormat;
    this->type = type;
}

/// @brief Generates a texture based on an image file
/// @param format format of the texture
/// @param internalFormat internal format of the texture
/// @param filePath path and file to use (ex. "/images/banana.png")
/// @param flip if the texture should be flipped - default true
void GL::Texture::GenTexture(TextureFormat format, GLuint internalFormat, std::string filePath, bool flip)
{
    this->Bind();
    this->path = filePath;
    
    if(loadedTextures.contains(filePath)) {
        Debug::LogSpam(std::format("Texture already loaded from path: {0}, using cached version", filePath));
        
        this->ID = loadedTextures[filePath];
        glBindTexture(GL_TEXTURE_2D, loadedTextures[filePath]);
        this->textureGenerated = true;
    }else{
        int width, height, nrChannels;
        unsigned char *data = Texture::LoadImageFromPath(filePath, width, height, nrChannels, flip);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, static_cast<GLuint>(format), GL_UNSIGNED_BYTE, data);
        if(this->generateMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

        Texture::FreeImageData(data);
        loadedTextures[filePath] = this->ID;
    }

    this->textureGenerated = true;

    this->format = format;
    this->internalFormat = internalFormat;
    this->type = GL_UNSIGNED_BYTE;
}

/// @brief Generates a texture from passed data
/// @param format format of the texture
/// @param internalFormat internal format of the texture
/// @param data the raw pixel data for the texture
/// @param width width of the texture
/// @param height height of the texture 
void GL::Texture::GenTexture(TextureFormat format, GLuint internalFormat, GLenum type, unsigned char *data, int width, int height)
{
    this->Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, static_cast<GLuint>(format), type, data);
    if(this->generateMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    this->textureGenerated = true;

    this->format = format;
    this->internalFormat = internalFormat;
    this->type = type;
}

void GL::Texture::Resize(int width, int height)
{
    if(!this->textureGenerated) [[unlikely]] {
        Debug::LogError("Resizing a texture that is not generated!");
        return;
    }

    if(this->path != "") [[unlikely]] {
        Debug::LogTrace(
            "Resizing a texture generated from a file path"
            "This may cause unexpected behavior"
            "texture path: " + this->path
        );
    }

    this->Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, width, height, 0, static_cast<GLuint>(this->format), this->type, nullptr);
}

void GL::Texture::Bind()
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}

/// @brief Binds the texture to a specific unit (ex. GL_TEXTURE4)
/// @param unit unit to bind to (max value = 31)
void GL::Texture::BindToUnit(uint8_t unit)
{
    if (!this->textureGenerated) [[unlikely]] {
        Debug::LogError("Binding a texture that is not generated!");
    }

    if(unit > 31) [[unlikely]] {
        Debug::LogError(std::format("Texture unit {0} is out of range. Max is 31. Binding to 0", unit));
        unit = 0;
    }

    glActiveTexture(GL_TEXTURE0 + unit);
    this->Bind();
}

/// @brief Loads raw pixel data from an image file
/// @param filePath path and file to use (ex. "/images/banana.png")
/// @param width reference to a width variable to load into
/// @param height reference to a height variable to load into
/// @param nrChannels reference to an nrChannels variable to load into
/// @param flip if the texture should be flipped - default true
/// @warning You MUST call `Texture::FreeImageData(data)` after you finish using them!
/// @return raw pixel data
unsigned char *GL::Texture::LoadImageFromPath(std::string filePath, int &width, int &height, int &nrChannels, bool flip)
{
    // check if file exists
    std::ifstream file(filePath);
    if(!file.good()) {
        Debug::LogError(std::format("Failed to load image from path: {0} (file does not exist)", filePath));
        width = 0;
        height = 0;
        nrChannels = 0;
        file.close();
        return nullptr;
    }
    file.close();

    stbi_set_flip_vertically_on_load(flip);
    return stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
}

/// @brief Frees raw image data from memory after `Texture::LoadImageFromPath(...)`
/// @param data data to be freed
void GL::Texture::FreeImageData(unsigned char *data)
{
    stbi_image_free(data);
}

GLuint GL::Texture::GetTextureFiltering(MipmapMode mipmap, bool blurred)
{
    if(mipmap == MipmapMode::None)
        return blurred ? GL_LINEAR : GL_NEAREST;

    if(mipmap == MipmapMode::Nearest)
        return blurred ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
    else // linear
        return blurred ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST;
}
