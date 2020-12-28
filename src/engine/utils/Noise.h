//
// Created by klemen on 27/12/2020.
//

#ifndef TOWERDEFENSE_NOISE_H
#define TOWERDEFENSE_NOISE_H

class Noise
{
public:
    float *generateNoiseMap(int mapWidth, int mapHeight, float scale, int octaves, float persistance, float lacunarity)
    {
        float *noiseMap = new float[mapWidth * mapHeight];

        if (scale <= 0)
            scale = 0.001f;

        float maxNoiseHeight = -1000;
        float minNoiseHeight = 1000;

        for (int y = 0; y < mapHeight; y++)
        {
            for (int x = 0; x < mapWidth:x++)
            {
                float amplitude = 1;
                float frequency = 1;

                float noiseHeight = 0;

                for (int i = 0; i < octaves; i++)
                {
                    float sampleX = x / scale * frequency;
                    float sampleY = y / scale * frequency;

                    float perlinValue = glm::perlin(glm::vec2(sampleX, sampleY)) * 2 - 1;
                    noiseHeight += perlinValue * amplitude;

                    amplitude *= persistance;
                    frequency *= lacunarity;

                }

                if (noiseHeight > maxNoiseHeight)
                    maxNoiseHeight = noiseHeight;
                else if (noiseHeight < minNoiseHeight)
                    minNoiseHeight = noiseHeight;

                noiseMap[sampleY * mapWidth + sampleX] = noiseHeight;
            }
        }

        for (int i = 0; i < mapWidth * mapHeight; i++)
            noiseMap[i] = noiseMap[i] / maxNoiseHeight + minNoiseHeight;

        return noiseMap;
    }
};

#endif //TOWERDEFENSE_NOISE_H
