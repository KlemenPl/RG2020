//
// Created by klemen on 06/12/2020.
//

#ifndef TOWERDEFENSE_RESOURCEMANAGER_H
#define TOWERDEFENSE_RESOURCEMANAGER_H

#include <unordered_map>
#include <string>
#include "graphics/Shader.h"
#include "graphics/Texture2D.h"

// singleton class for loading and managing resources (singleton so there will be only one instance)
class ResourceManager
{
private:
    inline static ResourceManager* instance = nullptr;

    std::unordered_map<std::string, Shader> shaders;
    std::unordered_map<std::string, Texture2D> textures;
    //std::unordered_map<std::string, Model> models;

public:
    static void init();

    virtual ~ResourceManager();

    // loading
    static void loadShader(const char* vsFile, const char* fsFile, const char* gsFile, std::string name,
                           ShaderSourceArgument args [] = nullptr, unsigned int argsLength=0);
    static void loadTexture(const char* textureFile, std::string name, bool alpha);

    // accessing
    static Shader& getShader(const std::string &name);
    static Texture2D getTexture2D(const std::string &name);

    // clear
    static void dispose();

    // no copy, move
    ResourceManager(ResourceManager const &) = delete;
    void operator=(ResourceManager const &) = delete;


private:
    ResourceManager()
    = default;
};


#endif //TOWERDEFENSE_RESOURCEMANAGER_H
