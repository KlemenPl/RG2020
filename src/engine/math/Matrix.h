//
// Created by klemen on 06/12/2020.
//

#ifndef TOWERDEFENSE_MATRIX_H
#define TOWERDEFENSE_MATRIX_H

#include <ostream>

template<int sizeX, int sizeY, typename T>
class Mat
{
public:
    mutable T vals[sizeX * sizeY];

    Mat() : vals{}
    {};

    explicit Mat(T fill) : vals{}
    {
        for (int i = 0; i < sizeX; i++)
            for (int j = 0; j < sizeY; j++)
                if(i==j)
                    vals[i*4+j] = fill;

    }

    explicit Mat(const T values[sizeX * sizeY]) : vals{}
    {
        for (int i = 0; i < sizeX * sizeY; i++)
            vals[i] = values[i];

    }

    explicit Mat(const T *values[sizeX * sizeY]) : vals{}
    {
        for (int i = 0; i < sizeX * sizeY; i++)
            vals[i] = values[i];

    }

    Mat(const Mat<sizeX, sizeY, T> &mat) : vals{}
    {
        for (int i = 0; i < sizeX * sizeY; i++)
            vals[i] = mat.vals[i];
    }

    T &operator[](int i) const
    {
        return (vals[i]);
    }

    T &getIndex(int i)
    {
        return &this[i];
    }
    T &getIndex(int i, int j)
    {
        return &this[i * j];
    }

    // addition
    Mat<sizeX, sizeY, T> operator+(const Mat<sizeX, sizeY, T> &mat) const
    {
        Mat<sizeX, sizeY, T> res;

        for (int i = 0; i < sizeX * sizeY; i++)
            res[i] = this[i] + mat[i];

        return res;
    }

    // single value addition
    Mat<sizeX, sizeY, T> operator+(const T value) const
    {
        Mat<sizeX, sizeY, T> res;

        for (int i = 0; i < sizeX * sizeY; i++)
            res[i] = this[i] + value;

        return res;
    }

    // add assign
    const Mat<sizeX,sizeY,T>& operator+=(const Mat<sizeX, sizeY, T> &mat) const
    {
        for (int i = 0; i < sizeX * sizeY; i++)
            this[i] = this[i] += mat[i];

        return *this;
    }


    // subtraction
    Mat<sizeX, sizeY, T> operator-(const Mat<sizeX, sizeY, T> &mat) const
    {
        Mat<sizeX, sizeY, T> res;

        for (int i = 0; i < sizeX * sizeY; i++)
            res[i] = this[i] - mat[i];

        return res;
    }

    // single value subtract
    Mat<sizeX, sizeY, T> operator-(const T value) const
    {
        Mat<sizeX, sizeY, T> res;

        for (int i = 0; i < sizeX * sizeY; i++)
            res[i] = this[i] - value;

        return res;
    }

    // subtract assign
    Mat<sizeX, sizeY, T> &operator-=(const Mat<sizeX, sizeY, T> &mat)
    {
        for (int i = 0; i < sizeX * sizeY; i++)
            this[i] = this[i] -= mat[i];

        return *this;
    }


    // multiplication
    Mat<sizeX, sizeY, T> operator*(const Mat<sizeX, sizeY, T> &mat) const
    {
        Mat<sizeX, sizeY, T> res;

        for (int i = 0; i < sizeX; i++)
        {
            for (int j = 0; j < sizeY; j++)
            {
                res[i * sizeX + j] = 0;
                for (int k = 0; k < sizeX; k++)
                    res[i * sizeX + j] += vals[i * sizeX + k] * mat[k * sizeY + j];
            }
        }

        return res;
    }

    // single value multiplication (scalar)
    Mat<sizeX, sizeY, T> operator*(const T value) const
    {
        Mat<sizeX, sizeY, T> res;
        for (int i = 0; i < sizeX * sizeY; i++)
            res[i] = vals[i] * value;

        return res;
    }

    // multiply assign
    Mat<sizeX, sizeY, T> &operator*=(const Mat<sizeX, sizeY, T> &mat)
    {
        Mat<sizeX, sizeY, T> res(*this, mat);

        for (int i = 0; i < sizeX * sizeY; i++)
            res[i] = vals[i] * res[i];
        return *this;
    }

    // division not needed


    // negation
    Mat<sizeX, sizeY, T> operator-() const
    {
        Mat<sizeX, sizeY, T> res;

        for (int i = 0; i < sizeX * sizeY; i++)
            res[i] = vals[i];

        return res;
    }

    // assignment (copy)
    Mat<sizeX, sizeY, T> &operator=(const Mat<sizeX, sizeY, T> &mat)
    {
        if (&mat != this)
            for (int i = 0; i < sizeX * sizeY; i++)
                vals[i] = mat[i];
        return *this;
    }

    // logic operators

    bool operator==(const Mat &rhs) const
    {
        return vals == rhs.vals;
    }
    bool operator!=(const Mat &rhs) const
    {
        return !(rhs == *this);
    }

    bool operator<(const Mat &rhs) const
    {
        return vals < rhs.vals;
    }
    bool operator>(const Mat &rhs) const
    {
        return rhs < *this;
    }
    bool operator<=(const Mat &rhs) const
    {
        return !(rhs < *this);
    }
    bool operator>=(const Mat &rhs) const
    {
        return !(*this < rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const Mat<sizeX, sizeY, T>& f)
    {
        for (int i = 0; i < sizeX * sizeY; i++)
        {
            if (i % sizeX == 0 && i > 0)
                os << "\n";
            os << f.vals[i] << "\t";
        }
        return os;
    }

};

// some common matrix types
typedef Mat<4, 4, float> Mat4f;
typedef Mat<3, 3, float> Mat3f;

#endif //TOWERDEFENSE_MATRIX_H
