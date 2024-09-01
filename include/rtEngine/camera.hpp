#pragma once

#include <rtEngine/component.hpp>

class Camera: public Component
{
public:
    float fov = 90.0;
    float focal_length = 4.4;
};