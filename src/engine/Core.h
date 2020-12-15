//
// Created by klemen on 15/12/2020.
//

#ifndef TOWERDEFENSE_CORE_H
#define TOWERDEFENSE_CORE_H

#include <memory>

// std::unique_ptr / std::shared_ptr is a mouthful to write
template<typename T>
using Ref = std::shared_ptr<T>; // refrence to be used anywhere

template<typename T>
using Scope = std::unique_ptr<T>; // only available in the scope

#endif //TOWERDEFENSE_CORE_H
