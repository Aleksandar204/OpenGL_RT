#include<rtEngine/engine.hpp>
#include <rtEngine/camera.hpp>
#include <rtEngine/time.hpp>
#include<rtEngine/mesh.hpp>

class Spin : public Component
{
    void Update() override
    {
        game_object->rotateLocalEuler(Time::getDeltaTime() * glm::vec3(0.0f,20.0f, 0.0f));
    }
};

int main()
{
    Engine e;
    e.addScene("game");
    e.setCurrentScene("game");

    GameObject* shrek = new GameObject("Dragon", glm::vec3(0.0f,1.5f,-4.0f),glm::vec3(0.0f,90.0f,0.0f),glm::vec3(4.0f,4.0f,4.0f));
    shrek->model = new Model("resources/models/dragon/dragon1percent.obj");
    // shrek->addComponent(new Spin());
    e.getCurrentScene()->addGameObject(shrek);

    GameObject* camera = new GameObject("Main Camera", glm::vec3(0.0f,2.0f,0.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(1.0f,1.0f,1.0f));
    camera->addComponent(new Camera());
    // camera->addComponent(new Spin());
    e.getCurrentScene()->addGameObject(camera);

    // GameObject* crate = new GameObject("Crate 1", glm::vec3(3.0f,1.3f,-7.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f));
    // crate->model = new Model("resources/models/container/untitled.obj");
    // // crate->addComponent(new Spin());
    // e.getCurrentScene()->addGameObject(crate);

    GameObject* crate2 = new GameObject("Crate 2", glm::vec3(-3.0f,1.3f,-5.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f));
    crate2->model = new Model("resources/models/container/untitled.obj");
    e.getCurrentScene()->addGameObject(crate2);

    GameObject* ground = new GameObject("Ground",glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f));
    ground->model = new Model("resources/models/ground/TestPlane.obj");
    e.getCurrentScene()->addGameObject(ground);

    e.run();
    return 0;
}