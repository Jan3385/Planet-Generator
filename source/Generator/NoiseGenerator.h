#pragma once

#include <glm/glm.hpp>

namespace Generator{
class ValueNoise{
private:
    uint32_t seed;
    inline float lerp(float a, float b, float t);
    inline float smoothstep(float t);
public:
    ValueNoise(uint32_t seed);
    ~ValueNoise();

    float GetNoise(glm::vec3 pos);
};
}