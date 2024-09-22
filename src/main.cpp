#include<rtEngine/engine.hpp>
#include <rtEngine/camera.hpp>
#include <rtEngine/time.hpp>
#include<rtEngine/mesh.hpp>

GLFWwindow* window2;

class FreeCam : public Component
{
    float speed = 5.0f, sensitivity = 0.15f;

    void Start() override
    {
        // game_object->translateGlobal(glm::vec3(-2.0f,2.0f,-0.5f));
    }

    void Update() override
    {
        if (glfwGetKey(window2, GLFW_KEY_W) == GLFW_PRESS)
            game_object->translateGlobal(glm::normalize(glm::vec3(game_object->getGlobalForward().x, 0.0f, game_object->getGlobalForward().z)) * glm::vec3(Time::getDeltaTime()*speed));
        if (glfwGetKey(window2, GLFW_KEY_S) == GLFW_PRESS)
            game_object->translateGlobal(glm::normalize(glm::vec3(game_object->getGlobalForward().x, 0.0f, game_object->getGlobalForward().z)) * glm::vec3(Time::getDeltaTime()*-speed));
        if (glfwGetKey(window2, GLFW_KEY_A) == GLFW_PRESS)
            game_object->translateLocal(glm::vec3(Time::getDeltaTime() * -speed,0.0f,0.0f));
        if (glfwGetKey(window2, GLFW_KEY_D) == GLFW_PRESS)
            game_object->translateLocal(glm::vec3(Time::getDeltaTime() * speed,0.0f,0.0f));
        if (glfwGetKey(window2, GLFW_KEY_SPACE) == GLFW_PRESS)
            game_object->translateGlobal(glm::vec3(0.0f,Time::getDeltaTime()*speed, 0.0f));
        if (glfwGetKey(window2, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            game_object->translateGlobal(glm::vec3(0.0f,-Time::getDeltaTime()*speed, 0.0f));
        if (glfwGetKey(window2, GLFW_KEY_Q) == GLFW_PRESS)
            game_object->rotateLocalEuler(Time::getDeltaTime() * glm::vec3(0.0f,100.0f, 0.0f));
        if (glfwGetKey(window2, GLFW_KEY_E) == GLFW_PRESS)
            game_object->rotateLocalEuler(Time::getDeltaTime() * glm::vec3(0.0f,-100.0f, 0.0f));
        if(glfwGetKey(window2, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window2, true);
    }
};



class Spin : public Component
{
    void Update() override
    {
        game_object->rotateLocalEuler(Time::getDeltaTime() * glm::vec3(0.0f,100.0f, 0.0f));
    }
};

int main()
{
    Engine e;
    e.addScene("game");
    e.setCurrentScene("game");
    window2 = e.m_renderer.window;

    GameObject* dragon = new GameObject("Dragon",glm::vec3(-7.0f,5.0f,-7.0f),glm::vec3(0.0f,-70.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f));
    dragon->model = new Model("resources/models/dragon/dragon_scaled.obj");
    e.getCurrentScene()->addGameObject(dragon);

    GameObject* shrek = new GameObject("Shrek",glm::vec3(7.0f,0.0f,-8.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f));
    shrek->model = new Model("resources/models/shrek/shrek_scaled.obj");
    e.getCurrentScene()->addGameObject(shrek);

    GameObject* crate = new GameObject("Wooden Crate", glm::vec3(2.0f,1.0f,-4.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f));
    crate->model = new Model("resources/models/container/untitled.obj");
    e.getCurrentScene()->addGameObject(crate);

    GameObject* crate2 = new GameObject("Wooden Crate 2", glm::vec3(0.0f,2.0f,0.0f),glm::vec3(0.0f,30.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f));
    crate2->model = new Model("resources/models/container/untitled.obj");
    crate->addChild(crate2);

    GameObject* ground_plane = new GameObject("Ground plane",glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f));
    ground_plane->model = new Model("resources/models/ground/TestPlane.obj");
    e.getCurrentScene()->addGameObject(ground_plane);

    GameObject* camera = new GameObject("Main Camera", glm::vec3(0.0f,5.0f,4.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(1.0f,1.0f,1.0f));
    camera->addComponent(new Camera());
    camera->addComponent(new FreeCam());
    e.getCurrentScene()->addGameObject(camera);

    



    e.run();
    return 0;
}