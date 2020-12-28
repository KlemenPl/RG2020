//
// Created by klemen on 15/12/2020.
//

#ifndef TOWERDEFENSE_LOADER_H
#define TOWERDEFENSE_LOADER_H

#include "../graphics/RawModel.h"

namespace Loader {
    // https://en.wikipedia.org/wiki/Wavefront_.obj_file
    RawModel *loadOBJ(const char *filePath, bool useIBO = true);
    void loadOBJObjects(const char *filePath, std::vector<std::pair<std::string,RawModel *>> &output, bool useIBO = true);
}
#endif //TOWERDEFENSE_LOADER_H
