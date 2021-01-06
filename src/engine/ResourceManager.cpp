//
// Created by klemen on 06/12/2020.
//

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include "ResourceManager.h"
#include "loader/Loader.h"
#include "graphics/Terrain.h"
#include "graphics/CubeMap.h"
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

    // shaders
    for (auto &shader:shaders)
        delete shader.second._get_ptr();
    ResourceManager::shaders.clear();

    // textures
    for (auto &texture:textures)
        delete texture.second._get_ptr();
    ResourceManager::textures.clear();

    // bitmap font cleanup
    for (auto &font:fonts)
        delete font.second._get_ptr();
    ResourceManager::fonts.clear();
}

std::string readFile(const char *path)
{
    std::ifstream file(path);
    if (!file.good())
        throw std::runtime_error("ResourceManager::loadShader() unable to find file to shader!");
    if (!file.is_open())
        throw std::runtime_error("ResourceManager::loadShader() unable to open file to shader!");
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
    {
        std::cout << "ResourceManager::applyArgument(): Invalid shader argument" << std::endl;
        return source;
    }

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

void ResourceManager::loadTexture(const char *textureFile, std::string name, bool alpha = true,
                                  uint32_t wrapS, uint32_t wrapT,
                                  uint32_t filterMin, uint32_t filterMag)
{
    Texture2D *texture2D = new Texture2D(wrapS, wrapT, filterMin, filterMag);
    if (alpha)
    {
        texture2D->internalFormat = GL_RGBA;
        texture2D->imageFormat = GL_RGBA;
    }
    // loading image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(textureFile, &width, &height, &nrChannels, 0);
    // generating texture
    if (data == nullptr)
    {
        std::cout << "ResourceManager::loadTexture(): Invalid file!" << std::endl;
        return;
    }
    texture2D->generate(width, height, data);
    // releasing allocated data
    stbi_image_free(data);

    instance->textures[name] = Ref<Texture2D>(texture2D);
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

    BitmapFont *font = new BitmapFont();
    font->generate(fontSize, atlasWidth, atlasHeight, padding, startChar, numChars, ttf_buffer, fsize);
    delete[] ttf_buffer;
    instance->fonts[name] = Ref<BitmapFont>(font);
}

void ResourceManager::loadModel(const char *modelFile, std::string name)
{
    RawModel *rawModel = Loader::loadOBJ(modelFile);
    rawModel->generateMeshes();
    instance->models[name] = Ref<RawModel>(rawModel);
}

void ResourceManager::loadCubeMap(const std::string &folderPath, const std::string &extension, std::string name)
{
    CubeMap *cubeMap = new CubeMap();
    std::vector<std::string> faces
            {
                    folderPath + "/right." + extension,
                    folderPath + "/left." + extension,
                    folderPath + "/top." + extension,
                    folderPath + "/bottom." + extension,
                    folderPath + "/front." + extension,
                    folderPath + "/back." + extension
            };
    cubeMap->loadCubeMap(faces);
    instance->cubemaps[name] = Ref<CubeMap>(cubeMap);
}

void ResourceManager::loadBiome(Biome *biome, std::string name)
{
    instance->biomes[name] = Ref<Biome>(biome);
}

Model ResourceManager::getModel(const std::string &name)
{
    return Model{instance->models[name].operator*()};
}

Ref<Shader> ResourceManager::getShader(const std::string &name)
{
    return instance->shaders[name];
}
Ref<Texture2D> ResourceManager::getTexture2D(const std::string &name)
{
    return instance->textures[name];
}

Ref<BitmapFont> ResourceManager::getFont(const std::string &name)
{
    return instance->fonts[name];
}

Ref<RawModel> ResourceManager::getRawModel(const std::string &name)
{
    return instance->models[name];
}

Ref<CubeMap> ResourceManager::getCubeMap(const std::string &name)
{
    return instance->cubemaps[name];
}
Ref<Biome> ResourceManager::getBiome(const std::string &name)
{
    return instance->biomes[name];
}
