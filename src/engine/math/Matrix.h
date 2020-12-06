//
// Created by klemen on 06/12/2020.
//

#ifndef TOWERDEFENSE_MATRIX_H
#define TOWERDEFENSE_MATRIX_H

template<int sizeX, int sizeY, typename T>
class Mat
{
public:
    mutable T vals[sizeX * sizeY];

    Mat() : vals{}
    {};

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
            vals[i] = mat.m_Vals[i];
    }

    T &operator[](int i)
    {
        return &(vals[i]);
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
    Mat<sizeX, sizeY, T> &operator+=(const Mat<sizeX, sizeY, T> &mat) const
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
                    res[i * sizeX + j] += this[i * sizeX + k] * mat[k * sizeY + j];
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
        for (int i = 0; i < sizeX * sizeY; i++)
            vals[i] = mat[i];
        return *this;
    }

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


};

#endif //TOWERDEFENSE_MATRIX_H
