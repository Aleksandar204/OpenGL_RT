#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <rtEngine/shader.hpp>
#include <rtEngine/scene.hpp>

struct RTMaterialInfo
{
    glm::vec3 albedo;
    glm::vec3 emmision_color;
    float emmision_strength;
    float smoothness;
};

struct RTMeshInfo
{
    int indices_start;
    int indices_num;
    RTMaterialInfo material;
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