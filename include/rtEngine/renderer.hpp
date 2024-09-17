#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <rtEngine/shader.hpp>
#include <rtEngine/scene.hpp>

struct RTMaterialInfo
{
    alignas(16) glm::vec3 albedo;
    alignas(16) glm::vec3 emmision_color;
    alignas(4)  float emmision_strength;
    alignas(4)  float smoothness;
    alignas(8)  float padding[2];
};

struct RTVertexInfo
{
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 normal;
    alignas(8) glm::vec2 tex_coords;
    alignas(8) float padding[2];
};

struct RTMeshInfo
{
    RTMaterialInfo material;
    int indices_start;
    int indices_num;
    GLuint64 diffuse_texture_handle;
    GLuint64 specular_texture_handle;
    GLuint64 normal_texture_handle;
    glm::mat4 mesh_matrix;
};

struct RTCameraInfo
{
    alignas(16) glm::vec3 camera_center;
    alignas(16) glm::vec3 look_at;
    alignas(4)  float fov;
    alignas(4)  float focal_length;
    alignas(8)  float padding[2];
};

class Renderer
{
public:
    int WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 720;

    bool use_raytracing = false;
    int sample_count = 4;
    int bounce_count = 4;

    Renderer();
    ~Renderer();

    void renderFrame(Scene* render_scene);
    void triggerResize();
    
    GLFWwindow *window;
    GLuint quad_texture;
    float resolution_scale = 1.0f;
private:
    const float quad_vertices[20] = {
        // viewport position ||   texture coords 3+2
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

    GLuint quad_vao, quad_vbo;
    Shader *quadShader, *renderShader;

    RTCameraInfo caminfo;
    GLuint m_vertex_ssbo, m_indices_ssbo, m_mesh_ssbo, m_camera_ubo;

    void renderRaytrace(Scene *render_scene);
    void renderRaster(Scene *render_scene);
};