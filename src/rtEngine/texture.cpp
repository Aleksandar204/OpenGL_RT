#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <rtEngine/texture.hpp>

Texture::Texture(const char *texturePath)
{
    path = std::string(texturePath);
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    //unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
    float *data = stbi_loadf(texturePath, &width, &height, &nrChannels, 0);

    GLenum format, internalformat;
    switch (nrChannels)
    {
    case 1:
        format = GL_RED;
        internalformat = GL_RED;
        break;
    case 2:
        format = GL_RG;
        internalformat = GL_RG32F;
        break;
    case 3:
        format = GL_RGB;
        internalformat = GL_RGB32F;
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
    case 4:
        format = GL_RGBA;
        internalformat = GL_RGBA32F;
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Transparent texture edges have artifacts around the edges when repeating, this should be GL_CLAMP but it messes up other things TODO: find solution
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
    default:
        throw std::runtime_error("Texture format not recognised");
    }

    if (data)
    {
        // std::cout << nrChannels << std::endl;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_FLOAT, data);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        glGenerateMipmap(GL_TEXTURE_2D);

        m_gpu_texture_handle = glGetTextureHandleARB(texture_id);
        glMakeTextureHandleResidentARB(m_gpu_texture_handle);
    }
    else
    {
        throw new std::runtime_error("Failed to load texture");
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}