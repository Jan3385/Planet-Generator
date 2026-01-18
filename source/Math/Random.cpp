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

/// @brief 
/// @param min minimum value (inclusive)
/// @param max maximum value (exclusive)
/// @return 
float Math::Random::GetFloat(float min, float max)
{
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

/// @brief 
/// @param min minimum value (inclusive) 
/// @param max maximum value (inclusive)
/// @return 
int32_t Math::Random::GetInt(int32_t min, int32_t max)
{
    std::uniform_int_distribution<int32_t> distribution(min, max);
    return distribution(generator);
}

bool Math::Random::GetBool()
{
    std::bernoulli_distribution distribution(0.5f);
    return distribution(generator);
}
