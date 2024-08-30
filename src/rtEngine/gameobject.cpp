#include <rtEngine/gameobject.hpp>

void GameObject::addChildrenRecursive(std::vector<GameObject *>& go_vector)
{
    for(auto child:children)
    {
        go_vector.push_back(child);
        child->addChildrenRecursive(go_vector);
    }
}