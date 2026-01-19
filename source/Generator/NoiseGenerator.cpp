#include "NoiseGenerator.h"

#include "Math/Random.h"

inline float Generator::ValueNoise::lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

inline float Generator::ValueNoise::smoothstep(float t)
{
    return t * t * (3.0f - 2.0f * t);
}

Generator::ValueNoise::ValueNoise(uint32_t seed)
{
    this->seed = seed;
}

Generator::ValueNoise::~ValueNoise()
{
}

float Generator::ValueNoise::GetNoise(glm::vec3 pos)
{
    glm::ivec3 cell = glm::floor(pos);
    glm::vec3 localPos = pos - glm::vec3(cell);

    float corners[2][2][2];
    for(int x = 0; x <= 1; x++){
        for(int y = 0; y <= 1; y++){
            for(int z = 0; z <= 1; z++){
                glm::ivec3 cornerPos = cell + glm::ivec3(x, y, z);
                uint32_t hash = Math::Random::HashFromPos(cornerPos, this->seed);
                corners[x][y][z] = Math::Random::FloatFromHash(hash, -1.0f, 1.0f);
            }
        }
    }
    float sx = smoothstep(localPos.x);
    float sy = smoothstep(localPos.y);
    float sz = smoothstep(localPos.z);

    float ix0 = lerp(
        lerp(corners[0][0][0], corners[1][0][0], sx),
        lerp(corners[0][1][0], corners[1][1][0], sx), 
        sy
    );

    float ix1 = lerp(
        lerp(corners[0][0][1], corners[1][0][1], sx),
        lerp(corners[0][1][1], corners[1][1][1], sx), 
        sy
    );

    float value = lerp(ix0, ix1, sz);

    return value;
}
