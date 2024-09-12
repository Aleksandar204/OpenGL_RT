#pragma once

#include <rtEngine/types.hpp>
#include <glad/glad.h>

#include <vector>


struct BVHBounds
{
    glm::vec3 bounds_min;
    glm::vec3 bounds_max;
    bool has_point = false;
    glm::vec3 getCenter()
    {
        return (bounds_max+bounds_min) / 2.0f;
    }
    glm::vec3 getSize()
    {
        return bounds_max-bounds_min;
    }

    /// @brief Calling this function grows the bounding box to include the entire triangle given the AABB bounds of the triangle
    /// @param tri_min Max bounds of the triangle eg. max(vert_a.position,vert_b.position,vert_c.position)
    /// @param tri_max Max bounds of the triangle eg. min(vert_a.position,vert_b.position,vert_c.position)
    void GrowToInclude(glm::vec3 tri_min,glm::vec3 tri_max)
    {
        if(has_point)
        {
            bounds_min = glm::min(bounds_min,tri_min);
            bounds_max = glm::max(bounds_max, tri_max);
        }
        else
        {
            has_point = true;
            bounds_min = tri_min;
            bounds_max = tri_max;
        }
    }
};

struct BVHNode
{
    glm::vec3 bounds_min;
    glm::vec3 bounds_max;
    /// @brief Index of first child node if triangle_count is negative
    int start_index;
    int triangle_count;

    /// @brief Constructor called for leaf nodes, where the triangle count is not set to -1
    /// @param bounds 
    /// @param index 
    /// @param count 
    BVHNode(BVHBounds bounds, int index, int count);

    /// @brief Constructor called for tree nodes, start_index and triangle_count are set to -1
    /// @param bounds 
    BVHNode(BVHBounds bounds);
};

class BVH
{
    std::vector<BVHNode> all_nodes;

    BVH(std::vector<Vertex>& mesh_vertices, std::vector<GLuint>& mesh_indices);
};