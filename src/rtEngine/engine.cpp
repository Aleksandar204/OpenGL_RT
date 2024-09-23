#include <rtEngine/engine.hpp>
#include <rtEngine/time.hpp>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

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
        ImGui::Text("FPS: %.2f\nFrametime: %.2f ms", 1/m_delta_time, m_delta_time * 1000);
        ImGui::InputInt("samples", &m_renderer.sample_count);
        ImGui::InputInt("bounces", &m_renderer.bounce_count);
        ImGui::Text("Raytracing render scale:");
        if(ImGui::SliderFloat("##", &m_renderer.resolution_scale, 0.01f, 1.0f))
        {
            m_renderer.triggerResize();
        }
        if(ImGui::Checkbox("Combine frames", &m_renderer.combine_frames))
        {
            m_renderer.resetCombinedFrames();
        }
        ImGui::End();

        ImGui::Begin("Scene", nullptr, 0);
        ImGui::Text("Current scene: %s", m_current_scene->name.c_str());

        for (auto obj : m_current_scene->game_objects)
        {
            if (ImGui::CollapsingHeader(obj->name.c_str()))
            {
                ImGui::BeginChild(obj->name.c_str(), ImVec2(0,0), ImGuiChildFlags_AutoResizeY);
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
    for (auto game_obj : m_current_scene->game_objects)
    {
        current_scene_gameobjects.push_back(game_obj);
        game_obj->addChildrenRecursive(current_scene_gameobjects);
    }
    for (auto game_obj : current_scene_gameobjects)
    {
        updateAndStart(game_obj);
    }
}

void Engine::updateAndStart(GameObject *obj)
{
    for (auto component : obj->components)
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

void Engine::setCurrentScene(std::string scene_name)
{
    for (auto scene : m_scenes)
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