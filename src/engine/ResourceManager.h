//
// Created by klemen on 06/12/2020.
//

#ifndef TOWERDEFENSE_RESOURCEMANAGER_H
#define TOWERDEFENSE_RESOURCEMANAGER_H

#include <unordered_map>
#include <string>
#include "graphics/Shader.h"

// singleton class for loading and managing resources (singleton so there will be only one instance)
class ResourceManager
{
public:
    std::unordered_map<std::string, Shader> shaders;
private:
    ResourceManager()
    = default;



};


#endif //TOWERDEFENSE_RESOURCEMANAGER_H
