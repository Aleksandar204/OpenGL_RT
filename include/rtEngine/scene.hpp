#pragma once

#include <vector>
#include <string>

#include <rtEngine/gameobject.hpp>

class Scene
{
private:
    bool m_gameobjects_changed = false;
public:
    std::string name;
    std::vector<GameObject *> game_objects;

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
};