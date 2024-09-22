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

    GameObject* dragon = new GameObject("Shrek",glm::vec3(-5.0f,1.0f,0.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(10.0f,10.0f,10.0f));
    dragon->model = new Model("resources/models/dragon/dragon1percent.obj");
    e.getCurrentScene()->addGameObject(dragon);

    // GameObject* ground = new GameObject("Ground",glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f));
    // ground->model = new Model("resources/models/CornellBox/CornellBox-Original.obj");
    // e.getCurrentScene()->addGameObject(ground);

    GameObject* groundtemp = new GameObject("Ground",glm::vec3(0.0f,-1.0f,0.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(1.0f,1.0f,1.0f));
    groundtemp->model = new Model("resources/models/ground/TestPlane.obj");
    e.getCurrentScene()->addGameObject(groundtemp);

    GameObject* camera = new GameObject("Main Camera", glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,90.0f,0.0f), glm::vec3(1.0f,1.0f,1.0f));
    camera->addComponent(new Camera());
    camera->addComponent(new FreeCam());
    e.getCurrentScene()->addGameObject(camera);

    



    e.run();
    return 0;
}