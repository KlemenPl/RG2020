//
// Created by klemen on 06/12/2020.
//

#ifndef TOWERDEFENSE_VECTOR_H
#define TOWERDEFENSE_VECTOR_H

// some serious template abuse :P

template<typename T>
struct vec_vals1
{
    T x;
};

template<typename T>
struct vec_vals2
{
    T x = 0;
    T y = 0;
};

template<typename T>
struct vec_vals3
{
    T x = 0;
    T y = 0;
    T z = 0;
};

template<typename T>
struct vec_vals4
{
    T x = 0;
    T y = 0;
    T z = 0;
    T w = 0;
};

#define vals(i) (float&)(&this->x)[i]

template<int length, typename T, typename V>
class Vector : public V
{
public:
    Vector() = default;

    explicit Vector(T _x)
    {
        this->x = _x;
    }

    explicit Vector(T _x, T _y)
    {
        this->x = _x;
        this->y = _y;
    }

    explicit Vector(T _x, T _y, T _w)
    {
        this->x = _x;
        this->y = _y;
        this->z = _w;
    }

    explicit Vector(T _x, T _y, T _w, T _z)
    {
        this->x = _x;
        this->y = _y;
        this->z = _w;
        this->w = _z;
    }

    T &operator[](int i) const
    {
        return vals(i);
    }

    // addition
    Vector<length, T, V> operator+(Vector<length, T, V> &vec) const
    {
        Vector<length, T, V> res;

        for (int i = 0; i < length; i++)
            res[i] = vals(i) + vec[i];

        return res;
    }

    // single value addition
    Vector<length, T, V> operator+(const T value) const
    {
        Vector<length, T, V> res;

        for (int i = 0; i < length; i++)
            res[i] = vals(i) + value;

        return res;
    }

    // add assign
    Vector<length, T, V> &operator+=(const Vector<length, T, V> &v)
    {
        for (int i = 0; i < length; i++)
            vals(i) += v[i];
        return *this;
    }


    // subtraction
    Vector<length, T, V> operator-(const Vector<length, T, V> &vec) const
    {
        Vector<length, T, V> res;

        for (int i = 0; i < length; i++)
            res[i] = vals(i) - vec[i];

        return res;
    }

    // single value subtract
    Vector<length, T, V> operator-(const T value) const
    {
        Vector<length, T, V> res;

        for (int i = 0; i < length; i++)
            res[i] = vals(i) - value;

        return res;
    }

    // subtract assign
    Vector<length, T, V> &operator-=(Vector<length, T, V> &v)
    {
        for (int i = 0; i < length; i++)
            vals(i) -= v[i];
        return *this;
    }


    // multiplication
    Vector<length, T, V> operator*(Vector<length, T, V> &vec) const
    {
        Vector<length, T, V> res;

        for (int i = 0; i < length; i++)
            res[i] = vals(i) * vec[i];

        return res;
    }

    // single value multiplication (scalar)
    Vector<length, T, V> operator*(const T value) const
    {
        Vector<length, T, V> res;

        for (int i = 0; i < length; i++)
            res[i] = vals(i) * value;

        return res;
    }

    // multiply assign
    Vector<length, T, V> &operator*=(Vector<length, T, V> &v)
    {
        for (int i = 0; i < length; i++)
            vals(i) *= v[i];
        return *this;
    }

    // division
    Vector<length, T, V> operator/(Vector<length, T, V> &vec) const
    {
        Vector<length, T, V> res;

        for (int i = 0; i < length; i++)
            res[i] = vals(i) / vec[i];

        return res;
    }

    // single value division
    Vector<length, T, V> operator/(const T value) const
    {
        Vector<length, T, V> res;

        for (int i = 0; i < length; i++)
            res[i] = vals(i) / value;

        return res;
    }

    // divide assign
    Vector<length, T, V> &operator/=(Vector<length, T, V> &v)
    {
        for (int i = 0; i < length; i++)
            vals(i) /= v[i];
        return *this;
    }


    // negation
    Vector<length, T, V> operator-() const
    {
        Vector<length, T, V> res;

        for (int i = 0; i < length; i++)
            res[i] = vals(i);

        return res;
    }

    // assignment (copy)
    Vector<length, T, V> &operator=(Vector<length, T, V> v)
    {
        for (int i = 0; i < length; i++)
            vals(i) = v[i];
        return *this;
    }

    bool operator==(Vector<length, T, V> &rhs) const
    {
        for (int i = 0; i < length; i++)
            if ((this->x)[i] != rhs[i])
                return false;
        return true;
    }
    bool operator!=(Vector<length, T, V> &rhs) const
    {
        return !(rhs==*this);
    }

    bool operator<(Vector<length, T, V> &rhs) const
    {
        for (int i = 0; i < length; i++)
            if ((this->x)[i] >= rhs[i])
                return false;
        return true;
    }
    bool operator>(Vector<length, T, V>&rhs) const
    {
        for (int i = 0; i < length; i++)
            if ((this->x)[i] <= rhs[i])
                return false;
        return true;
    }
    bool operator<=(Vector<length, T, V> &rhs) const
    {
        return !(rhs < *this);
    }
    bool operator>=(Vector<length, T, V> &rhs) const
    {
        return !(*this < rhs);
    }

    // add to stream
    friend std::ostream &operator<<(std::ostream &os,  Vector<length, T, V> vec)
    {
        for (int i = 0; i < length; i++)
            os << vec[i] << " ";
        return os;
    }
};

// some common vector usages
typedef Vector<1, float, vec_vals1<float>> Vec1f;
typedef Vector<2, float, vec_vals2<float>> Vec2f;
typedef Vector<3, float, vec_vals3<float>> Vec3f;
typedef Vector<4, float, vec_vals4<float>> Vec4f;

typedef Vector<1, int, vec_vals1<int>> Vec1i;
typedef Vector<2, int, vec_vals2<int>> Vec2i;
typedef Vector<3, int, vec_vals3<int>> Vec3i;
typedef Vector<4, int, vec_vals4<int>> Vec4i;

#endif //TOWERDEFENSE_VECTOR_H
