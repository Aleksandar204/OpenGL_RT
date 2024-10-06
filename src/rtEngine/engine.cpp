#include <rtEngine/engine.hpp>
#include <rtEngine/time.hpp>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include <stb/stb_image_write.h>

void Engine::run()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_renderer.window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    while (!glfwWindowShouldClose(m_renderer.window))
    {
        double current_frametime = glfwGetTime();
        m_delta_time = current_frametime - m_last_frametime;
        m_last_frametime = current_frametime;
        Time::setDeltaTime(m_delta_time);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui::ShowDemoWindow(); // Show demo window! :)

        ImGui::Begin("Renderer settings", nullptr, 0);
        ImGui::Checkbox("Use Raytracing", &m_renderer.use_raytracing);
        ImGui::Text("FPS: %.2f\nFrametime: %.2f ms", 1 / m_delta_time, m_delta_time * 1000);
        ImGui::InputInt("samples", &m_renderer.sample_count);
        ImGui::InputInt("bounces", &m_renderer.bounce_count);
        ImGui::Text("Raytracing render scale:");
        if (ImGui::SliderFloat("##", &m_renderer.resolution_scale, 0.01f, 1.0f))
        {
            m_renderer.triggerResize();
        }
        if (ImGui::Checkbox("Combine frames", &m_renderer.combine_frames))
        {
            m_renderer.resetCombinedFrames();
        }
        if (ImGui::Button("Export render"))
        {
            exportRender();
        }

        ImGui::End();

        ImGui::Begin("Scene", nullptr, 0);
        ImGui::Text("Current scene: %s", m_current_scene->name.c_str());

        for (auto const &obj : m_current_scene->game_objects)
        {
            if (ImGui::CollapsingHeader(obj->name.c_str()))
            {
                ImGui::BeginChild(obj->name.c_str(), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY);
                ImGui::Text("Position: ");
                ImGui::Text("%.3f %.3f %.3f", obj->getGlobalPosition().x, obj->getGlobalPosition().y, obj->getGlobalPosition().z);
                ImGui::Text("Rotation: ");
                ImGui::Text("%.3f %.3f %.3f", glm::degrees(obj->getGlobalRotationEuler().x), glm::degrees(obj->getGlobalRotationEuler().y), glm::degrees(obj->getGlobalRotationEuler().z));
                ImGui::Text("Scale: ");
                ImGui::Text("%.3f %.3f %.3f", obj->getGlobalScale().x, obj->getGlobalScale().y, obj->getGlobalScale().z);
                ImGui::EndChild();
            }
        }
        ImGui::End();

        startUpdateCurrentScene();

        m_renderer.renderFrame(m_current_scene);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_renderer.window);
        glfwPollEvents();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Engine::startUpdateCurrentScene()
{
    std::vector<GameObject *> current_scene_gameobjects;
    for (auto const &game_obj : m_current_scene->game_objects)
    {
        current_scene_gameobjects.push_back(game_obj);
        game_obj->addChildrenRecursive(current_scene_gameobjects);
    }
    for (auto const &game_obj : current_scene_gameobjects)
    {
        updateAndStart(game_obj);
    }
}

void Engine::updateAndStart(GameObject *obj)
{
    for (auto const &component : obj->components)
    {
        if (!component->started)
        {
            component->Start();
            component->started = true;
        }
        else
        {
            component->Update();
        }
    }
}

void Engine::addScene(std::string scene_name)
{
    m_scenes.push_back(new Scene(scene_name));
}

void Engine::removeScene(std::string scene_name)
{
    for(auto it = m_scenes.begin(); it != m_scenes.end(); ++it)
    {
        if((*it)->name == scene_name)
        {
            delete (*it);
            m_scenes.erase(it);
            return;
        }
    }
    throw std::runtime_error("Could not find scene with name " + scene_name);
}

void Engine::setCurrentScene(std::string scene_name)
{
    for (auto const &scene : m_scenes)
    {
        if (scene->name == scene_name)
        {
            m_current_scene = scene;
            return;
        }
    }
    throw std::runtime_error("Could not find scene with name " + scene_name);
}

Scene *Engine::getCurrentScene()
{
    return m_current_scene;
}

inline float linear_to_gamma(float linear_component)
{
    if (linear_component > 0)
        return std::min(std::sqrt(linear_component), 1.0f);

    return 0;
}

void Engine::exportRender()
{
    glBindTexture(GL_TEXTURE_2D, m_renderer.quad_texture);
    std::vector<float> pixels(m_renderer.getScaledWidth() * m_renderer.getScaledHeight() * 4);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels.data());
    std::vector<unsigned char> image(m_renderer.getScaledWidth() * m_renderer.getScaledHeight() * 4);
    for (int i = 0; i < m_renderer.getScaledWidth() * m_renderer.getScaledHeight() * 4; i++)
    {
        image[i] = static_cast<unsigned char>(linear_to_gamma(pixels[i]) * 255.0f);
    }
    stbi_flip_vertically_on_write(true);
    stbi_write_png("render_export.png", m_renderer.getScaledWidth(), m_renderer.getScaledHeight(), 4, image.data(), m_renderer.getScaledWidth() * 4);
    std::cout << "Exported image" << std::endl;
}

Engine::~Engine()
{
    std::cout << "Deleting engine" << std::endl;
    for(auto &scene : m_scenes)
    {
        delete scene;
    }
    std::cout << "Deleted engine" << std::endl;
}