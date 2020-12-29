//
// Created by klemen on 28/12/2020.
//

#ifndef TOWERDEFENSE_STRINGUTILS_H
#define TOWERDEFENSE_STRINGUTILS_H

#include <string>

namespace StringUtils {
    bool startsWith(const std::string &str, const std::string &start);
    std::string nextPart(const std::string &str, char separator, uint32_t &index);
}
#endif //TOWERDEFENSE_STRINGUTILS_H
