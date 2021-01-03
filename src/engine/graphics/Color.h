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

    // todo: consider using union
    /*
    union {
        struct
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };

        float packed;
    };
     */
    Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0) :
            r(r), g(g), b(b), a(a)
    {}

    static Color create(int r = 0, int g = 0, int b = 0, int a = 255)
    {
        return Color{r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
    }

    Color interpolate(const Color &c, float delta = 0.5f) const
    {
        Color out{};

        out.r = this->r + (delta * (c.r - this->r));
        out.g = this->g + (delta * (c.g - this->g));
        out.b = this->b + (delta * (c.b - this->b));
        out.a = this->a + (delta * (c.a - this->a));

        return out;
    }

    static Color interpolate(const Color &c1,const  Color &c2, float delta)
    {
        return c1.interpolate(c2,delta);
    }

    float pack() const
    {
        uint32_t packedColor = ((unsigned char) (255 * a) << 24) | ((unsigned char) (255 * b) << 16) |
                               ((unsigned char) (255 * g) << 8) | ((unsigned char) (255 * r));
        return *(float *) &packedColor; // type punning to float
    }


    static float pack(float r, float g, float b, float a)
    {
        uint32_t packedColor = ((unsigned char) (255 * a) << 24) | ((unsigned char) (255 * b) << 16) |
                               ((unsigned char) (255 * g) << 8) | ((unsigned char) (255 * r));
        return *(float *) &packedColor; // type punning to float
    }

    bool operator==(const Color &rhs) const
    {
        return r == rhs.r &&
               g == rhs.g &&
               b == rhs.b &&
               a == rhs.a;
    }
    bool operator!=(const Color &rhs) const
    {
        return !(rhs == *this);
    }

};

// some common colours
namespace Colors {
    // @formatter:off
    static const Color TRANSPARENT        {1.0f, 1.0f, 1.0f, 0.0f};
    static const Color WHITE              {1.0f, 1.0f, 1.0f, 1.0f};
    static const Color BLACK              {0.0f, 0.0f, 0.0f, 1.0f};
    static const Color RED                {1.0f, 0.0f, 0.0f, 1.0f};
    static const Color GREEN              {0.0f, 1.0f, 0.0f, 1.0f};
    static const Color BLUE               {0.0f, 0.0f, 1.0f, 1.0f};
    static const Color NAVY               {0.0f, 0.0f, 0.5f, 1.0f};
    static const Color MAGENTA            {1.0f, 0.0f, 1.0f, 1.0f};
    // @formatter:on
}


#endif //TOWERDEFENSE_COLOR_H
