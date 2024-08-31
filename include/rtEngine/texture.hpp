#pragma once

#include <glad/glad.h>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Texture
{
private:
    unsigned int texture_id;
    GLuint64 m_gpu_texture_handle;
public:
    std::string type;
    Texture(const char *texturePath);

    ~Texture()
    {
        glDeleteTextures(1,&texture_id);
    }

    void use()
    {
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }
    unsigned int get_id()
    {
        return texture_id;
    }

    GLuint64 getTextureHandle()
    {
        return m_gpu_texture_handle;
    }
};