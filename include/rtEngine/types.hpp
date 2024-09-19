#pragma once

#include<glm/glm.hpp>

struct Vertex // Padding and alignment included for ssbo positioning instead of using RTVertexInfo
{
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 normal;
    alignas(8) glm::vec2 tex_coords;
    alignas(8) float padding[2];
};