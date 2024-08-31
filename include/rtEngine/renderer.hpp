#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <rtEngine/shader.hpp>
#include <rtEngine/scene.hpp>

struct RTMaterialInfo
{
    float albedo[4];
    float emmision_color[4];
    float emmision_strength;
    float smoothness;
    float padding[2];
};

struct RTVertexInfo
{
    float position[4];
    float normal[4];
    float tex_coords[2];
    float padding[2];
};

struct RTMeshInfo
{
    RTMaterialInfo material;
    int indices_start;
    int indices_num;
    GLuint64 diffuse_texture_handle;
    GLuint64 specular_texture_handle;
    GLuint64 normal_texture_handle;
};

class Renderer
{
public:
    const int WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 720;

    Renderer();
    ~Renderer();

    void renderFrame(Scene* render_scene);

    GLFWwindow *window;

private:
    const float quad_vertices[20] = {
        // viewport position ||   texture coords 3+2
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

    unsigned int quad_vao, quad_vbo, quad_texture;
    Shader *quadShader, *renderShader;

    uint m_vertex_ssbo, m_indices_ssbo, m_mesh_ssbo;
};