#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <rtEngine/shader.hpp>

class Renderer
{
public:
    const int WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 720;

    Renderer();
    ~Renderer();

    void renderFrame();

    GLFWwindow *m_window;

private:
    const float quad_vertices[20] = {
        // viewport position ||   texture coords 3+2
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

    unsigned int quad_vao, quad_vbo, quad_texture;
    Shader *quadShader, *renderShader;
};