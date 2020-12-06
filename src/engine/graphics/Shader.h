//
// Created by klemen on 04/12/2020.
//

#ifndef TOWERDEFENSE_SHADER_H
#define TOWERDEFENSE_SHADER_H

#include <unordered_map>
#include <cstring>
#include <string>

enum ShaderType {
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    SHADER_PROGRAM
};

class Shader
{
private:
    unsigned int ID;
    std::unordered_map<std::string, unsigned int> uniformCache;

    // returns uniform location for shader (will also cache invalid uniforms)
    unsigned int getUniformLocation(const std::string &name);

public:
    // constructor
    Shader();

    ~Shader();

    // binds shader, and returns refrence for chaining
    Shader& bind();

    void compile(const char *vsSource, const char *fsSource, const char *gsSource = nullptr);

    bool validate(unsigned int object, ShaderType type);

    void setUniform(const std::string &name, float value);


};


#endif //TOWERDEFENSE_SHADER_H
