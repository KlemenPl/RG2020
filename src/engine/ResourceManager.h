//
// Created by klemen on 06/12/2020.
//

#ifndef TOWERDEFENSE_RESOURCEMANAGER_H
#define TOWERDEFENSE_RESOURCEMANAGER_H

#include <unordered_map>
#include <string>
#include <raudio.h>
#include "graphics/Shader.h"
#include "graphics/Texture2D.h"
#include "graphics/BitmapFont.h"

// singleton class for loading and managing resources (singleton so there will be only one instance)
class ResourceManager
{
private:
    inline static ResourceManager* instance = nullptr;

    std::unordered_map<std::string, Shader> shaders;
    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Music> musics;
    std::unordered_map<std::string, Sound> sounds;
    std::unordered_map<std::string, BitmapFont> fonts;
    //std::unordered_map<std::string, Model> models;

public:
    static void init();

    virtual ~ResourceManager();

    // loading
    static void loadShader(const char* vsFile, const char* fsFile, const char* gsFile, std::string name,
                           ShaderSourceArgument args [] = nullptr, uint32_t argsLength=0);
    static void loadTexture(const char* textureFile, std::string name, bool alpha);
    static void loadMusic(const char* musicFile, std::string name);
    static void loadSound(const char* soundFile, std::string name);
    static void loadFont(const char* fontFile, std::string name,
                         float fontSize = 64.0, int atlasWidth = 1024, int atlasHeight = 1024,
                         int padding = 2, int startChar = 32, int numChars = 95 );

    // accessing
    static Shader& getShader(const std::string &name);
    static Texture2D& getTexture2D(const std::string &name);
    static Music& getMusic(const std::string&name);
    static Sound& getSound(const std::string&name);
    static BitmapFont& getFont(const std::string&name);

    // clear
    static void dispose();

    // no copying as it is heavy object
    ResourceManager(ResourceManager const &) = delete;
    void operator=(ResourceManager const &) = delete;


private:
    ResourceManager()
    = default;
};


#endif //TOWERDEFENSE_RESOURCEMANAGER_H
