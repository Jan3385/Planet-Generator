#include "Color.h"

Math::RGB Math::RGB::operator*(const RGB &other) const
{
    return RGB(
        static_cast<uint8_t>(this->r * other.r / 255),
        static_cast<uint8_t>(this->g * other.g / 255),
        static_cast<uint8_t>(this->b * other.b / 255)
    );
}

Math::RGB Math::RGB::operator/(const RGB &other) const
{
    return RGB(
        static_cast<uint8_t>(this->r / other.r * 255),
        static_cast<uint8_t>(this->g / other.g * 255),
        static_cast<uint8_t>(this->b / other.b * 255)
    );
}

Math::RGB::RGB(Math::RGBA other)
    : RGB(other.r, other.g, other.b) { };

Math::RGB::RGB(uint8_t *c)
    : RGB(c[0], c[1], c[2]) { };

Math::RGB Math::RGB::Mix(RGB other, float t) const
{
    return RGB(
        static_cast<uint8_t>(this->r * (1 - t) + other.r * t),
        static_cast<uint8_t>(this->g * (1 - t) + other.g * t),
        static_cast<uint8_t>(this->b * (1 - t) + other.b * t)
    );
}

Math::RGBA Math::RGBA::operator*(const RGBA &other) const
{
    return RGBA(
        static_cast<uint8_t>(this->r * other.r / 255),
        static_cast<uint8_t>(this->g * other.g / 255),
        static_cast<uint8_t>(this->b * other.b / 255),
        static_cast<uint8_t>(this->a * other.a / 255)
    );
}

Math::RGBA Math::RGBA::operator/(const RGBA &other) const
{
    return RGBA(
        static_cast<uint8_t>(this->r / other.r * 255),
        static_cast<uint8_t>(this->g / other.g * 255),
        static_cast<uint8_t>(this->b / other.b * 255),
        static_cast<uint8_t>(this->a / other.a * 255)
    );
}

Math::RGBA::RGBA(uint8_t *c)
    : RGBA(c[0], c[1], c[2], c[3]) { };
