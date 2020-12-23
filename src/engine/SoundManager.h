//
// Created by klemen on 20/12/2020.
//

#ifndef TOWERDEFENSE_SOUNDMANAGER_H
#define TOWERDEFENSE_SOUNDMANAGER_H

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <string>
#include <iostream>
#include <vector>

//#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
//#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)



class SoundManager
{
public:
    SoundManager();
/*
    bool check_alc_errors(const std::string& filename, const std::uint_fast32_t line, ALCdevice* device)
    {
        ALCenum error = alcGetError(device);
        if(error != ALC_NO_ERROR)
        {
            std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n" ;
            switch(error)
            {
                case ALC_INVALID_VALUE:
                    std::cerr << "ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function";
                    break;
                case ALC_INVALID_DEVICE:
                    std::cerr << "ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function";
                    break;
                case ALC_INVALID_CONTEXT:
                    std::cerr << "ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function";
                    break;
                case ALC_INVALID_ENUM:
                    std::cerr << "ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function";
                    break;
                case ALC_OUT_OF_MEMORY:
                    std::cerr << "ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function";
                    break;
                default:
                    std::cerr << "UNKNOWN ALC ERROR: " << error;
            }
            std::cerr << std::endl;
            return false;
        }
        return true;
    }
    */


};


#endif //TOWERDEFENSE_SOUNDMANAGER_H
