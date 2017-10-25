#include "random.h"

const unsigned int PRIME32_1 = 2654435761U;
const unsigned int PRIME32_2 = 2246822519U;
const unsigned int PRIME32_3 = 3266489917U;
const unsigned int PRIME32_4 = 668265263U;
const unsigned int PRIME32_5 = 374761393U;

Random::Random(unsigned int seed, unsigned int min, unsigned int max) : seed(seed), min(min), max(max)
{
}

void Random::SetSeed(unsigned int seed)
{
    this->seed = seed;
    this->OnSetSeed();
}

float Random::GetNormalizedValue()
{
    return (GetValue() - min) / (float) (max - min);
}

glm::vec2 Random::GetNormalized2D()
{
    return glm::vec2(GetNormalizedValue(), GetNormalizedValue());
}

void MersenneRandom::OnSetSeed()
{
    this->r.seed(seed);
}

MersenneRandom::MersenneRandom(unsigned int seed) : Random(seed, r.min(), r.max())
{
    this->r.seed(seed);
}

unsigned int MersenneRandom::GetValue()
{
    return r();
}

Hash::Hash(unsigned int seed) : seed(seed)
{
}

XXHash::XXHash(unsigned int seed) : Hash(seed)
{
}

unsigned int RotateLeft (unsigned int  value, int count)
{
    return (value << count) | (value >> (32 - count));
}

// Reference implementation from: https://bitbucket.org/runevision/random-numbers-testing/src/113e3cdaf14a/Assets/Implementations/HashFunctions
unsigned int XXHash::Evaluate(unsigned int value)
{
    unsigned int h32 = seed + PRIME32_5;
    h32 += 4U;
    h32 += value * PRIME32_3;
    h32 = RotateLeft (h32, 17) * PRIME32_4;
    h32 ^= h32 >> 15;
    h32 *= PRIME32_2;
    h32 ^= h32 >> 13;
    h32 *= PRIME32_3;
    h32 ^= h32 >> 16;
    return h32;
}
