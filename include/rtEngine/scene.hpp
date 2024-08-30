#pragma once

#include <vector>
#include <string>

#include <rtEngine/gameobject.hpp>

class Scene
{
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
    }
};