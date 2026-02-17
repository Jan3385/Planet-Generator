#pragma once

#include <random>
#include <cstdint>

#include <glm/glm.hpp>

namespace Math{
class Random{
private:
    uint32_t seed;
    std::mt19937 generator;
public:
    static float FloatFromHash(uint32_t hash, float min, float max);
    static int32_t IntFromHash(uint32_t hash, int32_t min, int32_t max);
    static bool BoolFromHash(uint32_t hash);
    static uint32_t HashFromPos(const glm::vec3& pos, uint32_t seed);
    static uint32_t HashFromPos(const glm::ivec3& pos, uint32_t seed);
    static uint32_t HashFromInt(uint32_t value, uint32_t seed);
    static float HaltonSequence(unsigned int index, int base);

    Random();
    Random(uint32_t seed);
    ~Random();

    void SetSeed(uint32_t seed);
    uint32_t GetSeed() const { return seed; }

    float GetFloat(float min, float max);
    int32_t GetInt(int32_t min, int32_t max);
    bool GetBool();
    bool GetBool(float trueProbability);

    // disable copy
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;
    // move 
    Random(Random&&);
    Random& operator=(Random&&);
};
}