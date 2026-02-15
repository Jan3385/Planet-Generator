#pragma once

#include <glad/glad.h>
#include <string>
#include "GLWrapper/Texture.h"

namespace GL
{
class Cubemap{
public:
    Cubemap();
    Cubemap(const std::string filePaths[6], bool blurred, bool sRGB);
    ~Cubemap();

    // disable copy semantics
    Cubemap(const Cubemap&) = delete;
    Cubemap& operator=(const Cubemap&) = delete;

    // move semantics
    Cubemap(Cubemap&&other) noexcept;
    Cubemap& operator=(Cubemap&&other) noexcept;

    void Bind();
    static void Unbind() { glBindTexture(GL_TEXTURE_CUBE_MAP, 0); };

    bool isBlurred()  { return this->blurred; };
    bool isInitialized() { return this->ID != 0; };
protected:
    GLuint ID = 0;

    virtual void LoadCubemapFaces(const std::string filePaths[6], bool sRGB);
private:
    bool blurred;
};
}