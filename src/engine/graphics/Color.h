//
// Created by klemen on 08/12/2020.
//

#ifndef TOWERDEFENSE_COLOR_H
#define TOWERDEFENSE_COLOR_H

class Color
{
public:
    float r;
    float g;
    float b;
    float a;
    Color(float r, float g, float b, float a = 1.0) : r(r), g(g), b(b), a(a)
    {}

    float pack() const
    {
        unsigned int packedColor = ((unsigned char) (255 * a) << 24) | ((unsigned char) (255 * b) << 16) |
                                   ((unsigned char) (255 * g) << 8) | ((unsigned char) (255 * r));
        return *(float *)&packedColor; // returning same bits as packedColor as a float instead of uint
    }
};

#endif //TOWERDEFENSE_COLOR_H
