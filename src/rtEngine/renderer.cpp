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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(0, quad_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_FRAMEBUFFER_SRGB);

    glGenBuffers(1,&m_vertex_ssbo);
    glGenBuffers(1,&m_indices_ssbo);
    glGenBuffers(1,&m_mesh_ssbo);

    glGenBuffers(1,&m_camera_ubo);

    glBindBufferBase(GL_UNIFORM_BUFFER, 4, m_camera_ubo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_mesh_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_indices_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vertex_ssbo);

    caminfo.camera_center[0] = 0.0f;
    caminfo.camera_center[1] = 3.0f;
    caminfo.camera_center[2] = 0.0f;

    caminfo.look_at[0] = 0.0f;
    caminfo.look_at[1] = 3.0f;
    caminfo.look_at[2] = -1.0f;

    caminfo.focal_length = 1.0f;
    caminfo.fov = 70.0f;
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

    std::vector<RTVertexInfo> all_vertices;
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
            mesh_info.material.albedo[0] = 0.2f;
            mesh_info.material.albedo[1] = 0.75f;
            mesh_info.material.albedo[2] = 0.2f;
            mesh_info.material.emmision_color[0] = 0.0f;
            mesh_info.material.emmision_color[1] = 0.0f;
            mesh_info.material.emmision_color[2] = 0.0f;
            mesh_info.material.emmision_strength = 0.0f;
            mesh_info.material.smoothness = 0.0f;
            if(mesh.diffuse_maps.size() > 0)
                mesh_info.diffuse_texture_handle = mesh.diffuse_maps[0]->getTextureHandle();
            else
                mesh_info.diffuse_texture_handle = 0;
            if(mesh.specular_maps.size() > 0)
                mesh_info.specular_texture_handle = mesh.specular_maps[0]->getTextureHandle();
            else
                mesh_info.specular_texture_handle = 0;
            if(mesh.normal_maps.size() > 0)
                mesh_info.normal_texture_handle = mesh.normal_maps[0]->getTextureHandle();
            else
                mesh_info.normal_texture_handle = 0;
            all_meshes.push_back(mesh_info);
            for(auto vertex : mesh.vertices)
            {
                Vertex tmp;
                tmp.position = gameobj->getGlobalModelMatrix() * glm::vec4(vertex.position,1.0f);
                tmp.normal = glm::mat3(glm::transpose(glm::inverse(gameobj->getGlobalModelMatrix()))) * vertex.normal;
                tmp.tex_coords = vertex.tex_coords;
                RTVertexInfo v;

                v.position[0] = tmp.position.x;
                v.position[1] = tmp.position.y;
                v.position[2] = tmp.position.z;

                v.normal[0] = tmp.normal.x;
                v.normal[1] = tmp.normal.y;
                v.normal[2] = tmp.normal.z;

                v.tex_coords[0] = tmp.tex_coords.x;
                v.tex_coords[1] = tmp.tex_coords.y;

                all_vertices.push_back(v);
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

    caminfo.fov += 0.1f;

    glBindBuffer(GL_UNIFORM_BUFFER, m_camera_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(RTCameraInfo), &caminfo ,GL_STREAM_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_mesh_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(RTMeshInfo) * all_meshes.size(), all_meshes.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indices_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * all_indices.size(), all_indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_vertex_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(RTVertexInfo) * all_vertices.size(), all_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

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