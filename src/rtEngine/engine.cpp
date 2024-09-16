#include <rtEngine/engine.hpp>
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

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow(); // Show demo window! :)

        startUpdateCurrentScene();

        if (glfwGetKey(m_renderer.window, GLFW_KEY_R) == GLFW_PRESS)
        {
            m_renderer.use_raytracing = true;
        }
        else if (glfwGetKey(m_renderer.window, GLFW_KEY_T) == GLFW_PRESS)
        {
            m_renderer.use_raytracing = false;
        }

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