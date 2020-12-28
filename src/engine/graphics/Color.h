//
// Created by klemen on 08/12/2020.
//

#ifndef TOWERDEFENSE_COLOR_H
#define TOWERDEFENSE_COLOR_H

class Color
{
public:
    // todo: consider using union
    float r;
    float g;
    float b;
    float a;
    Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0) :
            r(r), g(g), b(b), a(a)
    {}

    float pack() const
    {
        uint32_t packedColor = ((unsigned char) (255 * a) << 24) | ((unsigned char) (255 * b) << 16) |
                               ((unsigned char) (255 * g) << 8) | ((unsigned char) (255 * r));
        return *(float *) &packedColor; // type punning to float
    }

    Color interpolate(Color &c, float delta = 0.5f)
    {
        Color out{};

        out.r = this->r + (delta * (c.r - this->r));
        out.g = this->g + (delta * (c.g - this->g));
        out.b = this->b + (delta * (c.b - this->b));
        out.a = this->a + (delta * (c.a - this->a));

        return out;
    }

    static float pack(float r, float g, float b, float a)
    {
        uint32_t packedColor = ((unsigned char) (255 * a) << 24) | ((unsigned char) (255 * b) << 16) |
                               ((unsigned char) (255 * g) << 8) | ((unsigned char) (255 * r));
        return *(float *) &packedColor; // type punning to float
    }

};

// some common colours
namespace Colors {
    // @formatter:off
    static const Color WHITE    {1.0f, 1.0f, 1.0f, 1.0f};
    static const Color BLACK    {0.0f, 0.0f, 0.0f, 1.0f};
    static const Color RED      {1.0f, 0.0f, 0.0f, 1.0f};
    static const Color GREEN    {1.0f, 1.0f, 1.0f, 1.0f};
    static const Color BLUE     {1.0f, 1.0f, 1.0f, 1.0f};
    // @formatter:on
}


#endif //TOWERDEFENSE_COLOR_H
