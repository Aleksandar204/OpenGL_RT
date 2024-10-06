#pragma once

#include <rtEngine/texture.hpp>
#include <rtEngine/shader.hpp>
#include <rtEngine/types.hpp>
#include<rtEngine/bvh.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class Mesh
{
private:
    GLuint m_vao,m_vbo,m_ebo;
public:
    Shader raster_shader = Shader("resources/shaders/basicUnlit.vert", "resources/shaders/basicUnlit.frag");
    Material material;
    BVH mesh_bvh;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture*> diffuse_maps;
    std::vector<Texture*> specular_maps;
    std::vector<Texture*> normal_maps;
    std::vector<Texture*> height_maps;
    std::vector<Texture*> emission_maps;

    Mesh(std::vector<Vertex> vert, std::vector<GLuint> ind, std::vector<Texture*> diffuse, std::vector<Texture*> specular, std::vector<Texture*> normalmap, std::vector<Texture*> height, std::vector<Texture*> emission);

    GLuint getVAO()
    {
        return m_vao;
    }
    void deleteMeshData();
};