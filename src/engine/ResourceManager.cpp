//
// Created by klemen on 06/12/2020.
//

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include "ResourceManager.h"
#include <stb_image.h>

void ResourceManager::init()
{
    if (ResourceManager::instance != nullptr)
        throw std::runtime_error("ResourceManager::init ResourceManager is already initialized.");

    ResourceManager::instance = new ResourceManager();

}

void ResourceManager::dispose()
{
    delete ResourceManager::instance;
}

ResourceManager::~ResourceManager()
{
    // freeing up memory
    //for (auto &shader:shaders)
    //    shader.second->~Shader();
    //for (auto &texture:textures)
    //    texture.second->~Texture2D();
    ResourceManager::shaders.clear();
    ResourceManager::textures.clear();

    // music cleanup
    for (auto &music:musics)
        UnloadMusicStream(*music.second);
    // sound cleanup
    for (auto &sound:sounds)
        UnloadSound(*sound.second);
    ResourceManager::musics.clear();
    ResourceManager::sounds.clear();

    // bitmap font cleanup
    //for (auto &font:fonts)
    //    font.second->~BitmapFont();
    ResourceManager::fonts.clear();
}

std::string readFile(const char *path)
{
    std::ifstream file(path);
    std::stringstream sstream;
    sstream << file.rdbuf();
    file.close();
    return sstream.str();
}

// probably not the best or most efficent way, but it's good enough
std::string applyArgument(const std::string &source, const ShaderSourceArgument &arg)
{
    std::string fullIdentifier = "{" + arg.identifier + "}";
    unsigned long startIndex = source.find(fullIdentifier);
    if (startIndex == std::string::npos)
        throw std::runtime_error("ResourceManager::applyArgument(): Invalid shader argument");

    unsigned long endIndex = startIndex + fullIdentifier.size();

    return source.substr(0, startIndex) + arg.argument + source.substr(endIndex, std::string::npos);
}


void ResourceManager::loadShader(const char *vsFile, const char *fsFile, const char *gsFile, std::string name,
                                 ShaderSourceArgument args[], uint32_t argsLength)
{
    std::string vertexSource = readFile(vsFile);
    std::string fragmentSource = readFile(fsFile);
    std::string geometrySource;

    bool geometryShaderPresent = false;
    if (gsFile != nullptr)
    {
        geometrySource = readFile(gsFile);
        geometryShaderPresent = true;
    }

    /*
     * Dynamically setting arguments of the shader instead of hard-coding
     */
    for (int i = 0; i < argsLength; i++)
    {
        auto &arg = args[i];
        switch (arg.shaderType)
        {
            case VERTEX:
                vertexSource = applyArgument(vertexSource, arg);
                break;
            case FRAGMENT:
                fragmentSource = applyArgument(fragmentSource, arg);
                break;
            case GEOMETRY:
                if (gsFile == nullptr)
                    throw std::runtime_error(
                            "ResourceMangaer::loadShader(): Can not apply argument on non valid shader.");
                geometrySource = applyArgument(geometrySource, arg);
                break;
            default:
                break;
        }

    }

    Shader *shader = new Shader;
    shader->compile(vertexSource.c_str(), fragmentSource.c_str(),
                    geometryShaderPresent ? geometrySource.c_str() : nullptr);
    instance->shaders[name] = Ref<Shader>(shader);
}

void ResourceManager::loadWhitePixelTexture()
{
    Texture2D *texture2D = new Texture2D;
    texture2D->internalFormat = GL_RGBA;
    texture2D->imageFormat = GL_RGBA;
    texture2D->filterMin = GL_LINEAR;
    texture2D->wrapS = GL_CLAMP_TO_EDGE;
    texture2D->wrapT = GL_CLAMP_TO_EDGE;

    int width = 2;
    int height = 2;
    unsigned char data[]{255, 255, 255, 255, 255, 255, 255, 255,
                         255, 255, 255, 255, 255, 255, 255, 255};
    texture2D->generate(width, height, data);

    instance->textures["sshape"] = Ref<Texture2D>(texture2D);
}

void ResourceManager::loadTexture(const char *textureFile, std::string name, bool alpha = true)
{
    Texture2D *texture2D = new Texture2D;
    if (alpha)
    {
        texture2D->internalFormat = GL_RGBA;
        texture2D->imageFormat = GL_RGBA;
    }
    // load image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(textureFile, &width, &height, &nrChannels, 0);
    // now generate texture
    texture2D->generate(width, height, data);
    // and finally free image data
    stbi_image_free(data);

    instance->textures[name] = Ref<Texture2D>(texture2D);
}

void ResourceManager::loadMusic(const char *musicFile, std::string name)
{
    instance->musics[name] = std::make_shared<Music>(LoadMusicStream(musicFile));
}
void ResourceManager::loadSound(const char *soundFile, std::string name)
{
    instance->sounds[name] = std::make_shared<Sound>(LoadSound(soundFile));
}

void ResourceManager::loadFont(const char *fontFile, std::string name, float fontSize, int atlasWidth, int atlasHeight,
                               int padding, int startChar, int numChars)
{
    // getting file length to allocate buffer
    FILE *f = fopen(fontFile, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    unsigned char *ttf_buffer = new unsigned char[fsize];
    fread(ttf_buffer, 1, fsize, fopen(fontFile, "rb"));

    BitmapFont *font = new BitmapFont;
    font->generate(fontSize, atlasWidth, atlasHeight, padding, startChar, numChars, ttf_buffer, fsize);
    delete[] ttf_buffer;
    instance->fonts[name] = Ref<BitmapFont>(font);
}

Ref<Shader> ResourceManager::getShader(const std::string &name)
{
    return instance->shaders[name];
}
Ref<Texture2D> ResourceManager::getTexture2D(const std::string &name)
{
    return instance->textures[name];
}

Ref<Music> ResourceManager::getMusic(const std::string &name)
{
    return instance->musics[name];
}
Ref<Sound> ResourceManager::getSound(const std::string &name)
{
    return instance->sounds[name];
}
Ref<BitmapFont> ResourceManager::getFont(const std::string &name)
{
    return instance->fonts[name];
}
