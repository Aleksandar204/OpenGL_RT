#pragma once

#include <rtEngine/texture.hpp>
#include <rtEngine/shader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

class Mesh
{
private:
    GLuint m_vao,m_vbo,m_ebo;
public:
    Shader raster_shader = Shader("resources/shaders/basicUnlit.vert", "resources/shaders/basicUnlit.frag");
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture*> diffuse_maps;
    std::vector<Texture*> specular_maps;
    std::vector<Texture*> normal_maps;
    std::vector<Texture*> height_maps;

    Mesh(std::vector<Vertex> vert, std::vector<GLuint> ind, std::vector<Texture*> diffuse, std::vector<Texture*> specular, std::vector<Texture*> normalmap, std::vector<Texture*> height);

    GLuint getVAO()
    {
        return m_vao;
    }
};