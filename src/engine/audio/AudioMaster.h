//
// Created by klemen on 21/12/2020.
//

#ifndef TOWERDEFENSE_AUDIOMASTER_H
#define TOWERDEFENSE_AUDIOMASTER_H

#include <AL/al.h>
#include <AL/alc.h>
#include <vector>

class AudioMaster
{
private:
    static std::vector<unsigned int> buffers;
public:
    AudioMaster() = default;

    static void init() {
        alcCreateContext();
    }

    static void loadSound(const char* file){
        unsigned int buffer = alGenBuffers();
        buffers.push_back(buffer);
    }

    static void dispose() {
        alcDestroyContext();
    }


};

#endif //TOWERDEFENSE_AUDIOMASTER_H
