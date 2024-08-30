#pragma once

class Component
{
public:
    virtual void Start() {}
    virtual void Update() {}
    bool started = false;
};