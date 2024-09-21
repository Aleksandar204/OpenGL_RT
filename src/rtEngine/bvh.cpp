#include<rtEngine/bvh.hpp>

BVH::BVH(std::vector<Vertex>& mesh_vertices, std::vector<GLuint>& mesh_indices)
{
    BVHBounds bounds;
    all_triangles.resize(mesh_indices.size() / 3);

    for(int i = 0; i< mesh_indices.size();i+=3)
    {
        Vertex a = mesh_vertices[mesh_indices[i + 0]];
        Vertex b = mesh_vertices[mesh_indices[i + 1]];
        Vertex c = mesh_vertices[mesh_indices[i + 2]];
        glm::vec3 center = (a.position+b.position+c.position) / 3.0f;
        glm::vec3 max = glm::max(glm::max(a.position,b.position),c.position);
        glm::vec3 min = glm::min(glm::min(a.position,b.position),c.position);
        all_triangles[i/3] = BVHTri(center, min, max, i);
        bounds.GrowToInclude(min,max);
    }
    all_nodes.push_back(BVHNode(bounds));
    splitNode(0, 0, all_triangles.size(), 0);
}

void BVH::splitNode(int parent_index, int triangle_index_offset,int triangle_num, int depth)
{
    const int max_depth = 8;
    if(depth >= max_depth)
    {
        all_nodes[parent_index].start_index = triangle_index_offset;
        all_nodes[parent_index].triangle_count = triangle_num;
    }
    else
    {
        glm::vec3 parent_size = all_nodes[parent_index].bounds_max - all_nodes[parent_index].bounds_min;
        int split_axis;
        if(parent_size.x > parent_size.y && parent_size.x > parent_size.z)
            split_axis = 0;
        else if(parent_size.y > parent_size.x && parent_size.y > parent_size.z)
            split_axis = 1;
        else
            split_axis = 2;

        float split_pos = ((all_nodes[parent_index].bounds_max + all_nodes[parent_index].bounds_min) / 2.0f)[split_axis];
        BVHBounds bounds_left,bounds_right;
        int num_in_left = 0;

        for(int i = triangle_index_offset; i < triangle_index_offset + triangle_num;i++)
        {
            BVHTri tri = all_triangles[i];
            if(tri.center[split_axis] < split_pos)
            {
                bounds_left.GrowToInclude(tri.min,tri.max);
                std::swap(all_triangles[i], all_triangles[triangle_index_offset+num_in_left]);
                num_in_left++;
            }
            else
            {
                bounds_right.GrowToInclude(tri.min,tri.max);
            }
        }

        int num_in_right = triangle_num - num_in_left;
        int tri_index_left = triangle_index_offset;
        int tri_index_right = tri_index_left + num_in_left;

        if(num_in_left <= 1 || num_in_right <= 1)
        {
            all_nodes[parent_index].start_index = triangle_index_offset;
            all_nodes[parent_index].triangle_count = triangle_num;
            return;
        }

        all_nodes[parent_index].start_index = all_nodes.size();
        int indexl = all_nodes.size();
        int indexr = indexl + 1;

        all_nodes.push_back(BVHNode(bounds_left));
        all_nodes.push_back(BVHNode(bounds_right));

        splitNode(indexl, tri_index_left, num_in_left, depth + 1);
        splitNode(indexr, tri_index_right, num_in_right, depth + 1);
    }
}


BVHNode::BVHNode(BVHBounds bounds, int index, int count)
{
    this->bounds_min = bounds.bounds_min;
    this->bounds_max = bounds.bounds_max;
    start_index = index;
    triangle_count = count;
}
BVHNode::BVHNode(BVHBounds bounds)
{
    this->bounds_min = bounds.bounds_min;
    this->bounds_max = bounds.bounds_max;
    start_index = -1;
    triangle_count = -1;
}
BVHTri::BVHTri(glm::vec3 center, glm::vec3 min, glm::vec3 max, int v_first_index)
{
    this->center = center;
    this->min = min;
    this->max = max;
    this->v_first_index = v_first_index;
}