#pragma once

#include <cstdint>

#include <array>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Math {
class RGBA;

class RGB{
public:
    uint8_t r;
    uint8_t g;
    uint8_t b;

    RGB operator*(const RGB& other) const;
    RGB operator/(const RGB& other) const;

    RGB() : r(0), g(0), b(0) {}
    RGB(Math::RGBA other);
    RGB(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue) {}

    // RGB format
    RGB(uint8_t *c);

    std::array<uint8_t, 3> ToArray3() const {
        return std::array<uint8_t, 3>{r, g, b};
    }

    RGB Mix(RGB other, float t) const;

    glm::vec3 ToVec3() const { return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f); }
};

class RGBA : public RGB {
public:
    uint8_t a;

    RGBA operator*(const RGBA& other) const;
    RGBA operator/(const RGBA& other) const;

    RGBA() : RGB(), a(255) {}
    RGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) 
        : RGB(red, green, blue), a(alpha) {}
    RGBA(RGB other) : RGB(other), a(255) {};
    RGBA(RGB other, uint8_t alpha) : RGB(other), a(alpha) {}

    std::array<uint8_t, 4> ToArray4() const {
        return std::array<uint8_t, 4>{r, g, b, a};
    }

    // RGBA format
    RGBA(uint8_t *c);

    glm::vec4 ToVec4() const { return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f); }
};
}