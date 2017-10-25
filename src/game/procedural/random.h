#ifndef RANDOM_H
#define RANDOM_H

#include "../../la.h"
#include <random>

class Hash
{
protected:
    unsigned int seed;

public:
    Hash(unsigned int seed);
    virtual unsigned int Evaluate(unsigned int value) = 0;
};

class XXHash : public Hash
{
public:
    XXHash(unsigned int seed);
    unsigned int Evaluate(unsigned int value);
};

// A simple wrapper for random generation in case we need to switch algorithms easily..
class Random
{
protected:
    unsigned int seed;
    unsigned int min;
    unsigned int max;

    virtual void OnSetSeed() = 0;

public:
    Random(unsigned int seed, unsigned int min, unsigned int max);
    void SetSeed(unsigned int seed);
    float GetNormalizedValue();
    glm::vec2 GetNormalized2D();

    virtual unsigned int GetValue() = 0;
};

// A wrapper for C++11 mersenne twister
class MersenneRandom : public Random
{
protected:
    std::mt19937 r;
    void OnSetSeed();

public:
    MersenneRandom(unsigned int seed);
   unsigned int GetValue();
};

#endif // RANDOM_H
