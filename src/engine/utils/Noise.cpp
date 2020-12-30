//
// Created by klemen on 30/12/2020.
//

#include "Noise.h"
#include <cmath>
#include <random>

float Noise::getPerlinNoise(int x, int y)
{
    float total = 0;
    float d = (float) pow(2, octaves - 1);
    for (int i = 0; i < octaves; i++)
    {
        float freq = (float) pow(roughness, i) * amplitude;
        total += getInterpolatedNoise(x * freq, y * freq) * amplitude;
    }
    return total;
}

float Noise::getSmoothNoise(int x, int y)
{
    float corners = (getNoise(x - 1, y - 1) + getNoise(x + 1, y - 1) + getNoise(x - 1, y + 1) +
                     getNoise(x + 1, y + 1)) / 16.0f;
    float sides = (getNoise(x - 1, y) + getNoise(x + 1, y) + getNoise(x, y - 1) + getNoise(x, y + 1)) / 8.0f;
    float center = getNoise(x, y) / 4.0f;
    return corners + sides + center;
}

float Noise::getNoise(int x, int y)
{
    std::mt19937 generator(x * 49632 + y * 325176 + seed);
    std::uniform_real_distribution<float> random(0, 1);
    return random(generator) * 2.0f - 1.0f;
}
float Noise::getInterpolatedNoise(float x, float y)
{
    int intX = (int) x;
    float fracX = x - intX;
    int intY = (int) y;
    float fracY = y - intY;

    float v1 = getSmoothNoise(intX, intY);
    float v2 = getSmoothNoise(intX + 1, intY);
    float v3 = getSmoothNoise(intX, intY + 1);
    float v4 = getSmoothNoise(intX + 1, intY + 1);
    float i1 = interpolate(v1, v2, fracX);
    float i2 = interpolate(v3, v4, fracX);
    return interpolate(i1, i2, fracY);
}
float Noise::interpolate(float a, float b, float blend)
{
    double theta = blend * 3.14159265359;
    float f = (float) ((1.0f - cos(theta)) * 0.5f);
    return a * (1 - f) + b * f;
}

long Noise::getSeed() const
{
    return seed;
}
int Noise::getOctaves() const
{
    return octaves;
}
float Noise::getAmplitude() const
{
    return amplitude;
}
float Noise::getRoughness() const
{
    return roughness;
}
