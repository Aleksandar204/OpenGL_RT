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

struct RTMeshInfo
{
    RTMaterialInfo material;
    int root_node_index;
    int indices_index_offset;
    GLuint64 diffuse_texture_handle;
    GLuint64 specular_texture_handle;
    GLuint64 normal_texture_handle;
    glm::mat4 mesh_matrix;
    glm::mat4 inverse_mesh_matrix;
    int vertex_index_offset;
    float padding[3];
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
    void resetCombinedFrames() {frames_num = 1;}
    
    GLFWwindow *window;
    GLuint quad_texture;
    float resolution_scale = 1.0f;
    bool combine_frames = false;
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
    GameObject* m_camera_gameobject = nullptr;
    GLuint m_vertex_ssbo, m_indices_ssbo, m_mesh_ssbo, m_bvh_node_ssbo, m_camera_ubo;

    std::vector<Vertex> m_all_scene_vertices;
    std::vector<int> m_all_scene_indices;
    std::vector<RTMeshInfo> m_all_scene_meshes;
    std::vector<BVHNode> m_all_scene_nodes;

    int frames_num = 1;

    void renderRaytrace(Scene *render_scene);
    void renderRaster(Scene *render_scene);
    void updateRaytraceBuffers(Scene *render_scene);
};