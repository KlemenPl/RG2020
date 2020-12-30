//
// Created by klemen on 30/12/2020.
//

#ifndef TOWERDEFENSE_NOISE_H
#define TOWERDEFENSE_NOISE_H

/*
 * Ported from:
 * https://github.com/TheThinMatrix/LowPolyWater/blob/master/terrain/generation/PerlinNoise.java
 */
class Noise
{
private:
    long seed{};
    int octaves{};
    float amplitude{};
    float roughness{};

public:
    Noise(long seed, int octaves, float amplitude, float roughness)
            : seed(seed), octaves(octaves), amplitude(amplitude), roughness(roughness)
    {}

    float getPerlinNoise(int x, int y);
    float getSmoothNoise(int x, int y);
    float getNoise(int x, int y);
    float getInterpolatedNoise(float x, float y);
    float interpolate(float a, float b, float blend);
    long getSeed() const;
    int getOctaves() const;
    float getAmplitude() const;
    float getRoughness() const;


};

#endif //TOWERDEFENSE_NOISE_H
