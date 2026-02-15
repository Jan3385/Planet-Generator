#include "Cubemap.h"

GL::Cubemap::Cubemap() : ID(0) { }

/// @brief Creates a cubemap texture from 6 image files
/// @param filePaths array of 6 file paths with file names (ex. ["/images/banana.png", ..])
/// @param blurred if the textures should be blurred
/// @param sRGB if the textures should be loaded within sRGB color space
/// @note Order of images: right, left, bottom, top, front, back
GL::Cubemap::Cubemap(const std::string filePaths[6], bool blurred, bool sRGB) 
    : blurred(blurred)
{
    glGenTextures(1, &ID);

    this->LoadCubemapFaces(filePaths, sRGB);
}

GL::Cubemap::~Cubemap()
{
    if(ID != 0) glDeleteTextures(1, &ID);
}

GL::Cubemap::Cubemap(Cubemap &&other) noexcept
{
    this->ID = other.ID;
    this->blurred = other.blurred;
    other.ID = 0;
}

GL::Cubemap &GL::Cubemap::operator=(GL::Cubemap &&other) noexcept
{
    if (this != &other) {
        if(this->ID != 0) glDeleteTextures(1, &this->ID);
        this->ID = other.ID;
        this->blurred = other.blurred;
        other.ID = 0;
    }
    return *this;
}

void GL::Cubemap::Bind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

/// @brief Loads all 6 cubemap faces from 6 image files
/// @param filePaths array of 6 file paths with file names (ex. ["/images/banana.png", ..])
/// @param sRGB if the textures should be loaded within sRGB color space
/// @note Order of images: right, left, bottom, top, front, back
void GL::Cubemap::LoadCubemapFaces(const std::string filePaths[6], bool sRGB)
{
    this->Bind();

    int width, height, nrChannels;
    unsigned char *data;

    GLuint internalFormat = sRGB ? GL_SRGB8 : GL_RGB8;
    GLuint filter = this->blurred ? GL_LINEAR : GL_NEAREST;

    for (uint8_t i = 0; i < 6; i++)
    {
        data = Texture::LoadImageFromPath(filePaths[i], width, height, nrChannels, true);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  
        
        Texture::FreeImageData(data);
    }
    
}
