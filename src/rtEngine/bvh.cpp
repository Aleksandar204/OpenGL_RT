#include<rtEngine/bvh.hpp>


BVH::BVH(std::vector<Vertex>& mesh_vertices, std::vector<GLuint>& mesh_indices)
{
    BVHBounds bounds;
    
}
BVHNode::BVHNode(BVHBounds bounds, int index, int count)
{
    bounds_max = bounds.bounds_max;
    bounds_min = bounds.bounds_min;
    start_index = index;
    triangle_count = count;
}
BVHNode::BVHNode(BVHBounds bounds)
{
    bounds_max = bounds.bounds_max;
    bounds_min = bounds.bounds_min;
    start_index = -1;
    triangle_count = -1;
}