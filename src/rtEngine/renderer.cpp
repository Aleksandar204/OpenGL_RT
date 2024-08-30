#include <rtEngine/renderer.hpp>

Renderer::Renderer()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Compute shader RT", nullptr, nullptr);

    if (window == nullptr)
    {
        glfwTerminate();
        throw new std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);
    // glfwSwapInterval(0);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw new std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);

    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    quadShader = new Shader("shaders/quad.vert", "shaders/quad.frag");
    renderShader = new Shader("shaders/raytrace.comp");
    glGenTextures(1, &quad_texture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, quad_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(0, quad_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_FRAMEBUFFER_SRGB);

    glGenBuffers(1,&m_vertex_ssbo);
    glGenBuffers(1,&m_indices_ssbo);
    glGenBuffers(1,&m_mesh_ssbo);
}

Renderer::~Renderer()
{
    delete quadShader;
    delete renderShader;
    glDeleteVertexArrays(1, &quad_vao);
    glDeleteBuffers(1, &quad_vbo);
    glfwTerminate();
}

void Renderer::renderFrame(Scene* render_scene)
{
    glClear(GL_COLOR_BUFFER_BIT);

    std::vector<Vertex> all_vertices;
    std::vector<int> all_indices;
    std::vector<RTMeshInfo> all_meshes;

    uint indices_mesh_start = 0;
    uint vertex_offset = 0;
    for(auto gameobj : render_scene->game_objects)
    {
        for(auto mesh : gameobj->model->meshes)
        {
            RTMeshInfo mesh_info;
            mesh_info.indices_start = indices_mesh_start;
            mesh_info.indices_num = mesh.indices.size();
            mesh_info.material.albedo = glm::vec3(0.1f);
            mesh_info.material.emmision_color = glm::vec3(0.0f);
            mesh_info.material.emmision_strength = 0.0f;
            mesh_info.material.smoothness = 0.0f;
            all_meshes.push_back(mesh_info);
            for(auto vertex : mesh.vertices)
            {
                Vertex vert;
                vert.position = gameobj->getGlobalModelMatrix() * glm::vec4(vertex.position, 1.0f);
                vert.normal = glm::transpose(glm::inverse(gameobj->getGlobalModelMatrix())) * glm::vec4(vertex.normal, 1.0f);
                vert.tex_coords = vertex.tex_coords;
                all_vertices.push_back(vert);
            }
            for(auto index : mesh.indices)
            {
                all_indices.push_back(index+vertex_offset);
            }
            indices_mesh_start += mesh.indices.size();
            vertex_offset += mesh.vertices.size();
        }
    }

    renderShader->use();
    renderShader->setInt("mesh_count", all_meshes.size());

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_mesh_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(RTMeshInfo) * all_meshes.size(), all_meshes.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_mesh_ssbo);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indices_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * all_indices.size(), all_indices.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_indices_ssbo);


    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_vertex_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vertex) * all_vertices.size(), all_vertices.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vertex_ssbo);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glDispatchCompute((unsigned int)WINDOW_WIDTH / 8, (unsigned int)WINDOW_HEIGHT / 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    quadShader->use();
    glBindVertexArray(quad_vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, quad_texture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
}