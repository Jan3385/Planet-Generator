#include "Random.h"

Math::Random::Random(Random &&other)
{
    this->seed = other.seed;
    this->generator = std::move(other.generator);
}

Math::Random &Math::Random::operator=(Math::Random &&other)
{
    if (this != &other)
    {
        this->seed = other.seed;
        this->generator = std::move(other.generator);
    }
    return *this;
}

inline uint32_t Scramble(uint32_t hash) 
{
    hash ^= 0x6a09e667;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;
    return hash;
}

/// @brief Convert hash to float in range [-1, 1]
/// @param hash input hash
/// @return float in range [-1, 1]
float Math::Random::FloatFromHash(uint32_t hash, float min, float max)
{
    hash = Scramble(hash);

    float f = (hash & 0xFFFFFF) / float(0xFFFFFF);
    return min + f * (max - min);
}

/// @brief Convert hash to int in range [min, max]
/// @param hash input hash
/// @param min minimum value (inclusive)
/// @param max maximum value (exclusive)
/// @return int in range [min, max]
int32_t Math::Random::IntFromHash(uint32_t hash, int32_t min, int32_t max)
{
    hash = Scramble(hash);

    return min + (hash % (max - min));
}

bool Math::Random::BoolFromHash(uint32_t hash)
{
    hash = Scramble(hash);

    return (hash & 1) != 0;
}

uint32_t Math::Random::HashFromPos(const glm::vec3 &pos, uint32_t seed)
{
    return (
        static_cast<uint32_t>(pos.x * 73856093) ^
        static_cast<uint32_t>(pos.y * 19349663) ^
        static_cast<uint32_t>(pos.z * 83492791) ^
        seed
    );
}

uint32_t Math::Random::HashFromPos(const glm::ivec3 &pos, uint32_t seed)
{
    return (
        static_cast<uint32_t>(pos.x * 73856093) ^
        static_cast<uint32_t>(pos.y * 19349663) ^
        static_cast<uint32_t>(pos.z * 83492791) ^
        seed
    );
}

uint32_t Math::Random::HashFromInt(uint32_t value, uint32_t seed)
{
    return (value * 2654435761) ^ seed;
}

/// @brief Generates a value in the Halton sequence for a given index and base
/// @param index The index in the sequence
/// @param base The base to use for the sequence (should be a prime number for better distribution)
/// @return A float in range of 0 (inclusive) to 1 (exclusive)
float Math::Random::HaltonSequence(unsigned int index, int base)
{
    float f = 1.0f;
    float r = 0.0f;

    while (index > 0) {
        f /= base;
        r += f * (index % base);
        index /= base;
    }
    return r;
}

Math::Random::Random()
{
    this->seed = rand();
    generator.seed(this->seed);
}

Math::Random::Random(uint32_t seed)
{
    this->seed = seed;
    generator.seed(this->seed);
}

Math::Random::~Random()
{
}

void Math::Random::SetSeed(uint32_t seed)
{
    this->seed = seed;
    generator.seed(this->seed);
}

/// @brief Returns a random float in the range [min, max)
/// @param min minimum value (inclusive)
/// @param max maximum value (exclusive)
/// @return 
float Math::Random::GetFloat(float min, float max)
{
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

/// @brief Returns a random int in the range [min, max]
/// @param min minimum value (inclusive) 
/// @param max maximum value (inclusive)
/// @return 
int32_t Math::Random::GetInt(int32_t min, int32_t max)
{
    std::uniform_int_distribution<int32_t> distribution(min, max);
    return distribution(generator);
}

/// @brief Returns a random bool with 50/50 chance
/// @return
bool Math::Random::GetBool()
{
    std::bernoulli_distribution distribution(0.5f);
    return distribution(generator);
}

/// @brief Returns a random bool with a given probability of being true
/// @param trueProbability Probability of the result being true (0.0f to 1.0f)
/// @return 
bool Math::Random::GetBool(float trueProbability)
{
    std::bernoulli_distribution distribution(trueProbability);
    return distribution(generator);
}
