//
// Created by klemen on 15/12/2020.
//

#ifndef TOWERDEFENSE_LOADER_H
#define TOWERDEFENSE_LOADER_H

#include "../graphics/Model.h"

namespace Loader {
    // https://en.wikipedia.org/wiki/Wavefront_.obj_file
    Model *loadOBJ(const char *filePath, bool recalculateNormals = false, bool ignoreVertexNormals = false);
}
#endif //TOWERDEFENSE_LOADER_H
