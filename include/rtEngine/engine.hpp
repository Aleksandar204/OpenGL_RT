#pragma once

#include <rtEngine/renderer.hpp>
#include <rtEngine/scene.hpp>

class Engine
{
public:
    void run();
    void addScene(std::string scene_name);
    void removeScene(std::string scene_name);
    void setCurrentScene(std::string scene_name);
    Scene* getCurrentScene();

private:
    Renderer m_renderer;
    std::vector<Scene *> m_scenes;
    Scene *m_current_scene = nullptr;

    void startUpdateCurrentScene();
    void updateAndStart(GameObject *obj);
};