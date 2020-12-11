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
    for (auto &shader:shaders)
        shader.second.dispose();
    for (auto &texture:textures)
        texture.second.dispose();
    ResourceManager::shaders.clear();
    ResourceManager::textures.clear();
}

std::string readFile(const char *path)
{
    std::ifstream file(path);
    std::stringstream sstream;
    sstream << file.rdbuf();
    file.close();
    return sstream.str();
}

// not the best way, but good enough
std::string applyArgument(std::string &source, ShaderSourceArgument &arg)
{
    std::string fullIdentifier = "{" + arg.identifier + "}";
    unsigned long startIndex = source.find(fullIdentifier);
    if (startIndex == std::string::npos)
        throw std::runtime_error("ResourceManager::applyArgument(): Invalid shader argument");

    unsigned long endIndex = startIndex + fullIdentifier.size();

    return source.substr(0, startIndex) + arg.argument + source.substr(endIndex, std::string::npos);


}


void ResourceManager::loadShader(const char *vsFile, const char *fsFile, const char *gsFile, std::string name,
                                 ShaderSourceArgument args[], unsigned int argsLength)
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

    Shader shader;
    shader.compile(vertexSource.c_str(), fragmentSource.c_str(),
                   geometryShaderPresent ? geometrySource.c_str() : nullptr);
    instance->shaders[name] = shader;

}
void ResourceManager::loadTexture(const char *textureFile, std::string name, bool alpha = true)
{
    Texture2D texture2D;
    if (alpha)
    {
        texture2D.internalFormat = GL_RGBA;
        texture2D.imageFormat = GL_RGBA;
    }
    // load image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(textureFile, &width, &height, &nrChannels, 0);
    // now generate texture
    texture2D.generate(width, height, data);
    // and finally free image data
    stbi_image_free(data);

    instance->textures[name] = texture2D;
}

Shader &ResourceManager::getShader(const std::string &name)
{
    return instance->shaders[name];
}
Texture2D ResourceManager::getTexture2D(const std::string &name)
{
    return instance->textures[name];
}