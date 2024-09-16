#include <rtEngine/renderer.hpp>
#include <rtEngine/camera.hpp>

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

    glEnable(GL_DEPTH_TEST);
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

    quadShader = new Shader("resources/shaders/quad.vert", "resources/shaders/quad.frag");
    renderShader = new Shader("resources/shaders/raytrace.comp");
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

    glGenBuffers(1, &m_vertex_ssbo);
    glGenBuffers(1, &m_indices_ssbo);
    glGenBuffers(1, &m_mesh_ssbo);

    glGenBuffers(1, &m_camera_ubo);

    glBindBufferBase(GL_UNIFORM_BUFFER, 4, m_camera_ubo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_mesh_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_indices_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vertex_ssbo);
}

Renderer::~Renderer()
{
    delete quadShader;
    delete renderShader;
    glDeleteVertexArrays(1, &quad_vao);
    glDeleteBuffers(1, &quad_vbo);

    glDeleteBuffers(1, &m_camera_ubo);
    glDeleteBuffers(1, &m_mesh_ssbo);
    glDeleteBuffers(1, &m_indices_ssbo);
    glDeleteBuffers(1, &m_vertex_ssbo);
    glfwTerminate();
}

void Renderer::renderFrame(Scene *render_scene)
{
    glClearColor(0.2, 0.8, 1.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (use_raytracing)
    {
        renderRaytrace(render_scene);
    }
    else
    {
        renderRaster(render_scene);
    }
}

void Renderer::renderRaytrace(Scene *render_scene)
{
    std::vector<RTVertexInfo> all_vertices;
    std::vector<int> all_indices;
    std::vector<RTMeshInfo> all_meshes;

    GLuint indices_mesh_start = 0;
    GLuint vertex_offset = 0;
    bool found_cam = false;

    std::vector<GameObject *> current_scene_gameobjects;
    for (auto gameobj : render_scene->game_objects)
    {
        current_scene_gameobjects.push_back(gameobj);
        gameobj->addChildrenRecursive(current_scene_gameobjects);
    }

    for (auto gameobj : current_scene_gameobjects)
    {
        if (!found_cam)
        {
            for (auto comp : gameobj->components)
            {
                if (Camera *c = dynamic_cast<Camera *>(comp))
                {
                    caminfo.camera_center = gameobj->getGlobalPosition();
                    caminfo.look_at = gameobj->getGlobalForward() + gameobj->getGlobalPosition();
                    caminfo.focal_length = c->focal_length;
                    caminfo.fov = c->fov;
                    found_cam = true;
                    break;
                }
            }
        }
        if (gameobj->model != nullptr)
        {
            for (auto mesh : gameobj->model->meshes)
            {
                RTMeshInfo mesh_info;
                mesh_info.mesh_matrix = gameobj->getGlobalModelMatrix();
                mesh_info.indices_start = indices_mesh_start;
                mesh_info.indices_num = mesh.indices.size();
                mesh_info.material.albedo.r = 0.5f;
                mesh_info.material.albedo.g = 0.5f;
                mesh_info.material.albedo.b = 0.5f;
                mesh_info.material.emmision_color[0] = 0.0f;
                mesh_info.material.emmision_color[1] = 0.0f;
                mesh_info.material.emmision_color[2] = 0.0f;
                mesh_info.material.emmision_strength = 0.0f;
                mesh_info.material.smoothness = 0.0f;
                if (mesh.diffuse_maps.size() > 0)
                {
                    mesh_info.diffuse_texture_handle = mesh.diffuse_maps[0]->getTextureHandle();
                    mesh_info.material.albedo.r = -1.0f;
                }
                if (mesh.specular_maps.size() > 0)
                {
                    mesh_info.specular_texture_handle = mesh.specular_maps[0]->getTextureHandle();
                    mesh_info.material.smoothness = -1.0f;
                }
                if (mesh.normal_maps.size() > 0)
                {
                    mesh_info.normal_texture_handle = mesh.normal_maps[0]->getTextureHandle();
                }
                all_meshes.push_back(mesh_info);
                for (auto vertex : mesh.vertices)
                {
                    RTVertexInfo v;

                    v.position = vertex.position;
                    v.normal = vertex.normal;
                    v.tex_coords = vertex.tex_coords;

                    all_vertices.push_back(v);
                }
                for (auto index : mesh.indices)
                {
                    all_indices.push_back(index + vertex_offset);
                }
                indices_mesh_start += mesh.indices.size();
                vertex_offset += mesh.vertices.size();
            }
        }
    }

    renderShader->use();
    renderShader->setInt("mesh_count", all_meshes.size());

    renderShader->setInt("sample_count", sample_count);
    renderShader->setInt("bounce_count", bounce_count);

    glBindBuffer(GL_UNIFORM_BUFFER, m_camera_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(RTCameraInfo), &caminfo, GL_STREAM_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_mesh_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(RTMeshInfo) * all_meshes.size(), all_meshes.data(), GL_STREAM_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indices_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * all_indices.size(), all_indices.data(), GL_STREAM_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_vertex_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(RTVertexInfo) * all_vertices.size(), all_vertices.data(), GL_STREAM_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glDispatchCompute((GLuint)WINDOW_WIDTH / 8, (GLuint)WINDOW_HEIGHT / 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    quadShader->use();
    glBindVertexArray(quad_vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, quad_texture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Renderer::renderRaster(Scene *render_scene)
{
    std::vector<GameObject *> current_scene_gameobjects;
    for (auto gameobj : render_scene->game_objects)
    {
        current_scene_gameobjects.push_back(gameobj);
        gameobj->addChildrenRecursive(current_scene_gameobjects);
    }

    bool found_cam = false;

    for (auto gameobj : current_scene_gameobjects)
    {
        if (!found_cam)
        {
            for (auto comp : gameobj->components)
            {
                if (Camera *c = dynamic_cast<Camera *>(comp))
                {
                    caminfo.camera_center = gameobj->getGlobalPosition();
                    caminfo.look_at = gameobj->getGlobalForward() + gameobj->getGlobalPosition();
                    caminfo.focal_length = c->focal_length;
                    caminfo.fov = c->fov;
                    found_cam = true;
                    break;
                }
            }
        }
    }
    for (auto gameobj : current_scene_gameobjects)
    {
        if (gameobj->model != nullptr)
        {
            for (auto mesh : gameobj->model->meshes)
            {
                mesh.raster_shader.use();
                mesh.raster_shader.setMat4("model", gameobj->getGlobalModelMatrix());
                mesh.raster_shader.setMat4("view", glm::lookAt(caminfo.camera_center, caminfo.look_at, glm::vec3(0,1,0)));
                mesh.raster_shader.setMat4("projection", glm::perspective(glm::radians(caminfo.fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f));

                mesh.raster_shader.setBindlessTexture("albedo_texture_handle", mesh.diffuse_maps[0]->getTextureHandle());

                glBindVertexArray(mesh.getVAO());
                glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }
    }
}