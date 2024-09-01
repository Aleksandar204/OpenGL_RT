#pragma once

class GameObject;

class Component
{
public:
    virtual void Start() {}
    virtual void Update() {}
    bool started = false;

    GameObject* game_object = nullptr;
};