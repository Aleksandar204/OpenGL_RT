#pragma once
#include <glad/glad.h>
#include<string>

class Shader
{
private:
    unsigned int shader_id;
    void checkCompileErrors(unsigned int shader, std::string type);
public:
    Shader(std::string vertexPath, std::string fragmentPath);
    Shader(std::string computePath);
    void use();
};