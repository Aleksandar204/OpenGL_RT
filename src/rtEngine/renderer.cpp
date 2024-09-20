#include <rtEngine/renderer.hpp>
#include <rtEngine/camera.hpp>
Renderer* current_renderer;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0,0,width,height);
    current_renderer->WINDOW_HEIGHT = height;
    current_renderer->WINDOW_WIDTH = width;

    glDeleteTextures(1,&current_renderer->quad_texture);
    glGenTextures(1,&current_renderer->quad_texture);
    glBindTexture(GL_TEXTURE_2D, current_renderer->quad_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, int(width * current_renderer->resolution_scale), int(height * current_renderer->resolution_scale), 0, GL_RGBA, GL_FLOAT, nullptr);    

    glBindImageTexture(0, current_renderer->quad_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::triggerResize()
{
    framebuffer_size_callback(window, WINDOW_WIDTH, WINDOW_HEIGHT);
}

Renderer::Renderer()
{
    current_renderer = this;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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
    glGenBuffers(1, &m_bvh_node_ssbo);

    glGenBuffers(1, &m_camera_ubo);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_bvh_node_ssbo);
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
    if(render_scene->getChangedFlag())
    {
        render_scene->resetChangedFlag();
        updateRaytraceBuffers(render_scene);
    }
    if(m_camera_gameobject == nullptr)
        throw std::runtime_error("Camera not found!");
    for(auto comp : m_camera_gameobject->components)
    {
        if(Camera *c = dynamic_cast<Camera *>(comp))
        {
            caminfo.camera_center = m_camera_gameobject->getGlobalPosition();
            caminfo.look_at = m_camera_gameobject->getGlobalForward() + m_camera_gameobject->getGlobalPosition();
            caminfo.focal_length = c->focal_length;
            caminfo.fov = c->fov;
            break;
        }
    }

    glBindBuffer(GL_UNIFORM_BUFFER, m_camera_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(RTCameraInfo), &caminfo, GL_DYNAMIC_DRAW);



    renderShader->use();

    renderShader->setInt("sample_count", sample_count);
    renderShader->setInt("bounce_count", bounce_count);

    glDispatchCompute((GLuint)(WINDOW_WIDTH*resolution_scale) / 8 +1, (GLuint)(WINDOW_HEIGHT*resolution_scale) / 8 +1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    quadShader->use();
    glBindVertexArray(quad_vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, quad_texture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Renderer::updateRaytraceBuffers(Scene *render_scene)
{
    m_all_scene_indices.clear();
    m_all_scene_meshes.clear();
    m_all_scene_nodes.clear();
    m_all_scene_vertices.clear();
    std::vector<GameObject *> current_scene_gameobjects;
    for (auto gameobj : render_scene->game_objects)
    {
        current_scene_gameobjects.push_back(gameobj);
        gameobj->addChildrenRecursive(current_scene_gameobjects);
    }
    GLuint vertices_offset = 0;
    GLuint indices_offset = 0;
    GLuint bvh_nodes_offset = 0;

    for(auto gameobj : current_scene_gameobjects)
    {
        if(m_camera_gameobject == nullptr)
        {
            for(auto comp : gameobj->components)
            {
                if(Camera *c = dynamic_cast<Camera *>(comp))
                {
                    m_camera_gameobject = gameobj;
                    break;
                }
            }
        }
        if(gameobj->model != nullptr)
        {
            for(auto mesh : gameobj->model->meshes)
            {
                RTMeshInfo mesh_info;
                // Set model matrices for mesh
                mesh_info.mesh_matrix = gameobj->getGlobalModelMatrix();
                mesh_info.inverse_mesh_matrix = glm::inverse(gameobj->getGlobalModelMatrix());

                // Set mesh material info for textureless objects and textures
                mesh_info.material.albedo = glm::vec3(0.7f);
                mesh_info.material.emmision_color = glm::vec3(0.0f);
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
                // if (mesh.normal_maps.size() > 0)
                // {
                //     mesh_info.normal_texture_handle = mesh.normal_maps[0]->getTextureHandle();
                // }

                // Set mesh BVH nodes, indices and vertices
                mesh_info.root_node_index = m_all_scene_nodes.size();
                mesh_info.indices_index_offset = indices_offset;
                mesh_info.vertex_index_offset = vertices_offset;

                m_all_scene_meshes.push_back(mesh_info);

                // Push all BVH nodes to buffer
                m_all_scene_nodes.insert(m_all_scene_nodes.end(), mesh.mesh_bvh.all_nodes.begin(), mesh.mesh_bvh.all_nodes.end());
                bvh_nodes_offset += mesh.mesh_bvh.all_nodes.size();

                // Push all indices to buffer
                m_all_scene_indices.insert(m_all_scene_indices.end(),mesh.indices.begin(), mesh.indices.end());
                indices_offset += mesh.indices.size();

                // Push all vertices to buffer
                m_all_scene_vertices.insert(m_all_scene_vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
                vertices_offset += mesh.vertices.size();
            }
        }
    }
    
    renderShader->use();
    renderShader->setInt("mesh_count", m_all_scene_meshes.size());

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_mesh_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(RTMeshInfo) * m_all_scene_meshes.size(), m_all_scene_meshes.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indices_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * m_all_scene_indices.size(), m_all_scene_indices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_vertex_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vertex) * m_all_scene_vertices.size(), m_all_scene_vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_bvh_node_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(BVHNode) * m_all_scene_nodes.size(), m_all_scene_nodes.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
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