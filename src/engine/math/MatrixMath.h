//
// Created by klemen on 09/12/2020.
//

#ifndef TOWERDEFENSE_MATRIXMATH_H
#define TOWERDEFENSE_MATRIXMATH_H

#include <cmath>
#include "Matrix.h"

namespace math {

    static Mat4f translate(const Mat4f &matrix, const Vec3f &axis)
    {
        Mat4f res(1.0);
        res[12] += axis[0];
        res[13] += axis[1];
        res[14] += axis[2];

        return res * matrix;
    }

    static Mat4f rotateX(const Mat4f &matrix, const float amount)
    {
        // 1    0   0
        // 0    cos -sin
        // 0    sin cos

        Mat4f res(1.0f);
        float cos = cosf(amount);
        float sin = sinf(amount);

        res.vals[5] = cos;
        res.vals[6] = -sin;

        res.vals[9]  = sin;
        res.vals[10] = cos;

        //res[5] = cos;
        //res[9] = -sin;

        //res[6] = sin;
        //res[10] = cos;

        return res * matrix;
    }

    static Mat4f rotateY(const Mat4f &matrix, float amount)
    {
        // cos  0   sin
        // 0    1   0
        // -sin  0   cos

        Mat4f res(1.0f);
        float cos = cosf(amount);
        float sin = sinf(amount);

        res.vals[0] = cos;
        res.vals[2] = sin;

        res.vals[8]  = -sin;
        res.vals[10] = cos;

        //res[0] = cos;
        //res[8] = sin;

        //res[2] = -sin;
        //res[10] = cos;

        return res * matrix;
    }

    static Mat4f rotateZ(const Mat4f &matrix, float amount)
    {
        // cos -sin 0
        // sin cos  0
        // 0   0    1

        Mat4f res(1.0f);
        float cos = cosf(amount);
        float sin = sinf(amount);

        res.vals[0] = cos;
        res.vals[1] = -sin;

        res.vals[4] = sin;
        res.vals[5] = cos;

        //res[0] = cos;
        //res[4] = -sin;

        //res[1] = sin;
        //res[5] = cos;


        return res * matrix;
    }

    static Mat4f scale(const Mat4f &matrix, const Vec3f &amount)
    {
        /*
        Mat4f res(1.0f);

        res[0] = matrix[0] * amount[0];
        res[1] = matrix[1] * amount[0];
        res[2] = matrix[2] * amount[0];
        res[3] = matrix[3] * amount[0];

        res[4] = matrix[4] * amount[1];
        res[5] = matrix[5] * amount[1];
        res[6] = matrix[6] * amount[1];
        res[7] = matrix[7] * amount[1];

        res[8] = matrix[8] * amount[2];
        res[9] = matrix[9] * amount[2];
        res[10] = matrix[10] * amount[2];
        res[11] = matrix[11] * amount[2];

        res[12] = matrix[12];
        res[13] = matrix[13];
        res[14] = matrix[14];
        res[15] = matrix[15];

        return res;
         */
        Mat4f res(1.0f);
        res[0] = amount[0];
        res[5] = amount[1];
        res[10] = amount[2];

        return res*matrix;
    }

    static Mat4f ortho(float left, float right, float bottom, float top, float near = -1.0f, float far = 1.0f)
    {
        Mat4f res(1.0f);
        res[0] = 2.0f / (right - left);
        res[5] = 2.0f / (top - bottom);
        res[10] = 2.0f / (far - near);
        res[12] = -(right + left) / (right - left);
        res[13] = -(top + bottom) / (top - bottom);
        res[14] = -(far + near) / (far - near);

        return res;
    }
}
#endif //TOWERDEFENSE_MATRIXMATH_H
