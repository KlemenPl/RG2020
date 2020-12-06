//
// Created by klemen on 04/12/2020.
//

#include <glad/glad.h>
#include <iostream>
#include "Shader.h"

Shader::Shader()
{
}

Shader::~Shader()
{
    // cleaning up
    glDeleteProgram(ID);
}

Shader &Shader::bind()
{
    glUseProgram(ID);

    return *this;
}

unsigned int Shader::getUniformLocation(const std::string &name)
{
    if (uniformCache.find(name) != uniformCache.end())
        // in our cache
        return uniformCache[name];
    else
    {
        // not in our cache, caching and returning
        unsigned int location = glGetUniformLocation(ID, name.c_str());
        uniformCache[name] = location;
        return location;
    }
}


void Shader::compile(const char *vsSource, const char *fsSource, const char *gsSource)
{
    unsigned int vertexID, fragmentID, geometryID;

    // vertex shader
    vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &vsSource, NULL);
    glCompileShader(vertexID);

}

bool Shader::validate(unsigned int object, ShaderType type)
{
    int status;
    glGetProgramiv(object, type == SHADER_PROGRAM ? GL_LINK_STATUS : GL_COMPILE_STATUS, &status);

    if (status == 0)
    {
        // failed to validate

        int errorLength;
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &errorLength);

        // message
        char errorMessage[errorLength];
        glGetShaderInfoLog(object, errorLength, &errorLength, errorMessage);
        std::cout << "[ERROR::SHADER]\t";
        if (type == SHADER_PROGRAM)
            std::cout << "Link-time error:\n";
        else
            std::cout << "Compile-timer error:\n";
        std::cout << errorMessage << std::endl;

        return false;
    }

    return true;
}
void Shader::setUniform(const std::string &name, float value)
{
    glUniform1f(getUniformLocation(name), value);
}
