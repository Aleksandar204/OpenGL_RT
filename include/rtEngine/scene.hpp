#pragma once

#include <vector>
#include <string>

#include <rtEngine/gameobject.hpp>

class Scene
{
private:
    bool m_gameobjects_changed = false;
    Texture *m_skybox = nullptr;
public:
    std::string name;
    std::vector<GameObject *> game_objects;

    glm::vec3 default_sky_color = glm::vec3(0.2f, 0.8f, 1.0f);

    Scene(std::string scene_name)
    {
        name = scene_name;
    }
    void addGameObject(GameObject *game_object)
    {
        game_objects.push_back(game_object);
        m_gameobjects_changed = true;
    }
    void resetChangedFlag()
    {
        m_gameobjects_changed = false;
    }
    bool getChangedFlag()
    {
        return m_gameobjects_changed;
    }
    void setSkybox(std::string filepath)
    {
        if(m_skybox != nullptr)
        {
            delete m_skybox;
        }
        m_skybox = new Texture(filepath.c_str());
    }
    Texture* getSkyboxTexture()
    {
        return m_skybox;
    }
};