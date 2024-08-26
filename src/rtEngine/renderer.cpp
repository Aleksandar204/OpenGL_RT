#include <rtEngine/renderer.hpp>

Renderer::Renderer()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    m_window = glfwCreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,"Compute shader RT", nullptr, nullptr);

    if (m_window == nullptr)
    {
        glfwTerminate();
        throw new std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
            throw new std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glGenVertexArrays(1,&quad_vao);
    glGenBuffers(1,&quad_vbo);

    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    quadShader = new Shader("shaders/quad.vert","shaders/quad.frag");
    renderShader = new Shader("shaders/raytrace.comp");
    glGenTextures(1,&quad_texture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, quad_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(0, quad_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D,0);

    glEnable(GL_FRAMEBUFFER_SRGB);
}

Renderer::~Renderer()
{
    delete quadShader;
    delete renderShader;
    glDeleteVertexArrays(1, &quad_vao);
    glDeleteBuffers(1, &quad_vbo);
    glfwTerminate();
}

void Renderer::renderFrame()
{
    renderShader->use();
    glDispatchCompute((unsigned int)WINDOW_WIDTH/8,(unsigned int)WINDOW_HEIGHT/8,1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    
    quadShader->use();
    glBindVertexArray(quad_vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, quad_texture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0,4);
    glBindVertexArray(0);
}