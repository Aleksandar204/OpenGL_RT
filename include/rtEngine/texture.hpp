#pragma once

#include <glad/glad.h>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Texture
{
private:
    GLuint texture_id;
    GLuint64 m_gpu_texture_handle;
public:
    std::string path;
    std::string type;
    Texture(const char *texturePath);

    ~Texture()
    {
        std::cout << "Deleting texture " + path << std::endl;
        glMakeTextureHandleNonResidentARB(m_gpu_texture_handle);
        glDeleteTextures(1,&texture_id);
        std::cout << "Deleted texture " + path << std::endl;
    }

    void use()
    {
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }
    GLuint getID()
    {
        return texture_id;
    }

    GLuint64 getTextureHandle()
    {
        return m_gpu_texture_handle;
    }
};