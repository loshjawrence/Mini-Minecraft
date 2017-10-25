#ifndef PERLIN_H
#define PERLIN_H

#include "la.h"
const uint32_t SCALE2D = 3;
const uint32_t SCALE3D = 5;
const uint32_t OCTAVES = 6;
const double PERSISTANCE = 1 / 2.0;
enum INTERP_TYPE {LINEAR,COSINE,CUBIC};
const uint32_t INTERP_METHOD = CUBIC;

class Perlin {
public:
    Perlin();
    ~Perlin();
    static float noise1D(const int32_t x);
    static float noise2D(const int32_t x, const int32_t y);
    static float noise3D(const int32_t x, const int32_t y, const int32_t z);

    static float interpolate(const float, const float, const float, const float, const float);
    static float linearInterpolate(const float, const float, const float);
    static float cosineInterpolate(const float, const float, const float);
    static float cubicInterpolate(const float, const float, const float, const float, const float);

    static float smoothedNoise1D(const int32_t);
    static float smoothedNoise2D(const int32_t,const int32_t);
    static float smoothedNoise3D(const int32_t,const int32_t,const int32_t);

    static float interpolatedNoise1D(const float);
    static float interpolatedNoise2D(const float, const float);
    static float interpolatedNoise3D(const float, const float, const float);

    static float perlinNoise1D(const float);
    static float perlinNoise2D(const float, const float);
    static float perlinNoise3D(const float, const float, const float);

    static float clamp(const float, const float, const float);
};

#endif // PERLIN_H
