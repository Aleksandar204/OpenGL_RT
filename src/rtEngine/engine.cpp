#include <rtEngine/engine.hpp>

int cnt_tmp = 0;

void Engine::run()
{
    while (!glfwWindowShouldClose(m_renderer.window))
    {
        double current_frametime = glfwGetTime();
        m_delta_time = current_frametime - m_last_frametime;
        m_last_frametime = current_frametime;
        if(cnt_tmp == 10)
        {
            std::cout << 1/m_delta_time << std::endl;
            cnt_tmp = 0;
        }
        cnt_tmp++;
        startUpdateCurrentScene();

        m_renderer.renderFrame(m_current_scene);
        glfwPollEvents();
    }
}

void Engine::startUpdateCurrentScene()
{
    std::vector<GameObject*> current_scene_gameobjects;
    for(auto game_obj : m_current_scene->game_objects)
    {
        current_scene_gameobjects.push_back(game_obj);
        game_obj->addChildrenRecursive(current_scene_gameobjects);
    }
    for(auto game_obj : current_scene_gameobjects)
    {
        updateAndStart(game_obj);
    }
}

void Engine::updateAndStart(GameObject *obj)
{
    for(auto component: obj->components)
    {
        if(!component->started)
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
    for(auto scene:m_scenes)
    {
        if(scene->name == scene_name)
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