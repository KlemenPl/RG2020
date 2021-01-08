//
// Created by klemen on 15/12/2020.
//

#ifndef TOWERDEFENSE_CORE_H
#define TOWERDEFENSE_CORE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

#define DEBUG_MODE 1 // todo set to 0

/*
 * Similar to std::shared_pointer. It can be copied
 * without copying the refrence.
 *
 * Needs manual cleanup.
 */
template<typename T>
class Ref
{
private:
    T *refrence;
public:
    Ref() : refrence(nullptr)
    {}
    explicit Ref(T *refrence) : refrence(refrence)
    {}

    T *operator->()
    {
        return refrence;
    }

    const T *operator->() const
    {
        return refrence;
    }

    T &operator*()
    {
        return *refrence;
    }

    T *_get_ptr()
    {
        return refrence;
    }
    T *_get_ptr() const
    {
        return refrence;
    }

    T &_get_ref()
    {
        return *refrence;
    }
    T &_get_ref() const
    {
        return *refrence;
    }

    const T *_getRefrence() const
    {
        return refrence;
    }

    bool operator==(const Ref &rhs) const
    {
        return refrence == rhs.refrence;
    }
    bool operator!=(const Ref &rhs) const
    {
        return !(rhs == *this);
    }


};

namespace std {

    template<typename T>
    struct hash<Ref<T>>
    {

        std::size_t operator()(const Ref<T> &k) const
        {
            using std::hash;
            return hash<uint32_t>()(k._get_ptr());
        }
    };

}

// std::unique_ptr / std::shared_ptr is a mouthful to write
template<typename T>
using SmartRef = std::shared_ptr<T>; // refrence to be used anywhere

template<typename T>
using Scope = std::unique_ptr<T>; // only available in the scope

#endif //TOWERDEFENSE_CORE_H
