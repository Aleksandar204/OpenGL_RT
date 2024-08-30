#include<rtEngine/engine.hpp>

#include<rtEngine/mesh.hpp>

int main()
{
    Engine e;
    e.addScene("game");
    e.setCurrentScene("game");

    GameObject* crate = new GameObject(glm::vec3(0.0f,0.0,-5.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.5f,1.5f,1.5f));
    crate->model = new Model("models/container/untitled.obj");
    e.getCurrentScene()->addGameObject(crate);

    e.run();
    return 0;
}