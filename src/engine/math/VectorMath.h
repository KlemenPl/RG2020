//
// Created by klemen on 09/12/2020.
//

#ifndef TOWERDEFENSE_VECTORMATH_H
#define TOWERDEFENSE_VECTORMATH_H

namespace math {

    template<int length1, typename T, typename K>
    float length(const Vector<length1, T, K>& v)
    {
        float sum = 0;
        for (int i = 0; i < length1; i++)
            sum += v[i] * v[i];

        return sqrtf(sum);
    }

    template<int length, typename T, typename K>
    Vector<length, T, K> normalize(const Vector<length, T, K>& v)
    {
        float l = math::length(v);
        if (l == 0)
            l++;

        Vector<length, T, K> res;
        for (int i = 0; i < length; i++)
        {
            res[i] = v[i] / l;
        }

        return res;
    }

    template<int length, typename T, typename K>
    T dot(const Vector<length, T, K>& v1, const Vector<length, T, K>& v2)
    {
        T sum = 0;
        for (int i = 0; i < length; i++)
            sum += v1[i] * v2[i];

        return sum;
    }

    template<typename T, typename K>
    Vector<3,T, K> cross(const Vector<3,T, K>& v1, const Vector<3,T, K>& v2)
    {
        /*
         * return new Vector4f(
            (v1.y * v2.z - v1.z * v2.y),
            (v1.z * v2.x - v1.x * v2.z),
            (v1.x * v2.y - v1.y * v2.x),
            v1.w
        );
         */

        Vector<3,T, K> res;
        res[0] = (v1[1] * v2[2] - v1[2] * v2[1]);
        res[1] = (v1[2] * v2[0] - v1[0] * v2[2]);
        res[2] = (v1[0] * v2[1] - v1[1] * v2[0]);

        return res;

    }
}
#endif //TOWERDEFENSE_VECTORMATH_H
