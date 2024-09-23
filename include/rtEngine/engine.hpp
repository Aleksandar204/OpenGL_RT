#pragma once

#include <rtEngine/renderer.hpp>
#include <rtEngine/scene.hpp>
#include <rtEngine/input.hpp>

class Engine
{
public:
    void run();
    void quit() {glfwSetWindowShouldClose(m_renderer.window, true);};
    void addScene(std::string scene_name);
    void removeScene(std::string scene_name);
    void setCurrentScene(std::string scene_name);
    Scene* getCurrentScene();
    Renderer m_renderer;
    Input input = Input(this->m_renderer.window);
private:
    double m_delta_time, m_last_frametime;

    
    std::vector<Scene *> m_scenes;
    Scene *m_current_scene = nullptr;

    void startUpdateCurrentScene();
    void updateAndStart(GameObject *obj);
};