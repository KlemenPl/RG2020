//
// Created by klemen on 04/01/2021.
//

#ifndef TOWERDEFENSE_CUBEMAP_H
#define TOWERDEFENSE_CUBEMAP_H

#include "../Core.h"
#include <vector>

class CubeMap
{
private:
    uint32_t ID;
    inline static uint32_t VAO = 0;
    inline static uint32_t VBO = 0;
    inline static uint32_t IBO = 0;

    inline static uint32_t numUses = 0;
public:
    CubeMap();

    void bind() const;
    void unbind()const;

    virtual ~CubeMap();

    void loadCubeMap(std::vector<std::string> &faces);
};


#endif //TOWERDEFENSE_CUBEMAP_H
