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
 * Similar to std::shared_pointer. It can be copied without copying the refrence,
 * needs manual cleenup
 */
template<typename T>
class Ref
{
private:
    T *refrence;
public:
    Ref() : refrence(nullptr)
    {}
    Ref(T *refrence) : refrence(refrence)
    {}

    T *operator->()
    {
        return refrence;
    }

    T &operator*()
    {
        return *refrence;
    }


};

// std::unique_ptr / std::shared_ptr is a mouthful to write
template<typename T>
using SmartRef = std::shared_ptr<T>; // refrence to be used anywhere

template<typename T>
using Scope = std::unique_ptr<T>; // only available in the scope

#endif //TOWERDEFENSE_CORE_H
