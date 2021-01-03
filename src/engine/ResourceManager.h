//
// Created by klemen on 06/12/2020.
//

#ifndef TOWERDEFENSE_RESOURCEMANAGER_H
#define TOWERDEFENSE_RESOURCEMANAGER_H

#include <unordered_map>
#include <string>
#include "Core.h"
#include "graphics/Shader.h"
#include "graphics/Texture2D.h"
#include "graphics/BitmapFont.h"

// singleton class for loading and managing resources (singleton so there will be only one instance)
class ResourceManager
{
private:
    inline static ResourceManager* instance = nullptr;

    std::unordered_map<std::string, Ref<Shader>> shaders;
    std::unordered_map<std::string, Ref<Texture2D>> textures;
    //std::unordered_map<std::string, Ref<Music>> musics;
    //std::unordered_map<std::string, Ref<Sound>> sounds;
    std::unordered_map<std::string, Ref<BitmapFont>> fonts;
    //std::unordered_map<std::string, Ref<Model>> models;

public:
    static void init();

    virtual ~ResourceManager();

    // loading
    static void loadShader(const char* vsFile, const char* fsFile, const char* gsFile, std::string name,
                           ShaderSourceArgument args [] = nullptr, uint32_t argsLength=0);
    static void loadTexture(const char* textureFile, std::string name, bool alpha,
                            uint32_t wrapS = GL_REPEAT,
                            uint32_t wrapT = GL_REPEAT,
                            uint32_t filterMin = GL_LINEAR,
                            uint32_t filterMag = GL_LINEAR
                            );
    static void loadWhitePixelTexture();
    static void loadFont(const char* fontFile, std::string name,
                         float fontSize = 64.0, int atlasWidth = 1024, int atlasHeight = 1024,
                         int padding = 2, int startChar = 32, int numChars = 95 );

    // accessing
    static Ref<Shader> getShader(const std::string &name);
    static Ref<Texture2D> getTexture2D(const std::string &name);
    //static Ref<Music> getMusic(const std::string&name);
    //static Ref<Sound> getSound(const std::string&name);
    static Ref<BitmapFont> getFont(const std::string&name);

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
