#include <cstdint>
#include <glm/vec3.hpp>

namespace Math {
class RGB{
public:
    uint8_t r;
    uint8_t g;
    uint8_t b;

    RGB(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue) {}
    glm::vec3 ToVec3() const { return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f); }
};
}