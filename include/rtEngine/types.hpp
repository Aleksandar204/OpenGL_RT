#pragma once

#include<glm/glm.hpp>

struct Vertex // Padding and alignment included for ssbo positioning instead of using RTVertexInfo
{
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 normal;
    alignas(8) glm::vec2 tex_coords;
    alignas(8) float padding[2];
};

struct Material
{
    alignas(16) glm::vec3 albedo;
    alignas(16) glm::vec3 emmision_color;
    alignas(4)  float emmision_strength;
    alignas(4)  float smoothness;
    alignas(8)  float padding[2];

    Material() // Default white material
    {
        this->albedo = glm::vec3(0.8f);
        this->emmision_strength = 0.0f;
        this->emmision_color = glm::vec3(0.0f);
        this->smoothness = 0.0f;
    }
};