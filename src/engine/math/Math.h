//
// Created by klemen on 09/12/2020.
//

#ifndef TOWERDEFENSE_MATH_H
#define TOWERDEFENSE_MATH_H
namespace math {
    const long double PIld = 3.14159265358979323846264338327950288419716939937510582L;
    const float PI = (float) PIld;

    static float toRad(float degrees)
    {
        return degrees * (PI / 180.0f);
    }

    static float toDeg(float radians)
    {
        return radians / (PI / 180.0f);
    }
}
#endif //TOWERDEFENSE_MATH_H
