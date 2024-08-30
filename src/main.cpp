#include<rtEngine/engine.hpp>

#include<rtEngine/mesh.hpp>

int main()
{
    Engine e;
    e.addScene("game");
    e.setCurrentScene("game");

    GameObject* shrek = new GameObject(glm::vec3(-2.0f,0.0f,-5.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(4.0f,4.0f,4.0f));
    shrek->model = new Model("models/shrek/shrek.obj");
    e.getCurrentScene()->addGameObject(shrek);

    GameObject* crate = new GameObject(glm::vec3(3.0f,1.0f,-5.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f));
    crate->model = new Model("models/container/untitled.obj");
    e.getCurrentScene()->addGameObject(crate);

    GameObject* ground = new GameObject(glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f));
    ground->model = new Model("models/ground/TestPlane.obj");
    e.getCurrentScene()->addGameObject(ground);

    e.run();
    return 0;
}