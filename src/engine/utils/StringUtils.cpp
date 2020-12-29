//
// Created by klemen on 28/12/2020.
//

#include "StringUtils.h"

namespace StringUtils {
    bool startsWith(const std::string &str, const std::string &start)
    {
        unsigned int length = start.length();

        if (length > str.length())
            return false;

        for (int i = 0; i < length; i++)
            if (str[i] != start[i])
                return false;

        return true;
    }

    std::string nextPart(const std::string &str, char separator, uint32_t &index)
    {
        int i;
        for (i = index; i < str.length(); i++)
            if (str[i] == separator)
                break;
        std::string part = str.substr(index, i - index);
        for (; i < str.length(); i++)
            if (str[i] != separator)
                break;
        index = i;
        return part;
    }
}