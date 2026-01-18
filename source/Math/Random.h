#pragma once

#include <random>
#include <cstdint>

namespace Math{
class Random{
private:
    uint32_t seed;
    std::mt19937 generator;
public:
    Random();
    Random(uint32_t seed);
    ~Random();

    void SetSeed(uint32_t seed);
    uint32_t GetSeed() const { return seed; }

    float GetFloat(float min, float max);
    int32_t GetInt(int32_t min, int32_t max);
    bool GetBool();

    // disable copy
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;
    // move 
    Random(Random&&);
    Random& operator=(Random&&);
};
}