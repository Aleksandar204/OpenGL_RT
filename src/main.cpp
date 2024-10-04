#include <rtEngine/engine.hpp>
#include <rtEngine/camera.hpp>
#include <rtEngine/time.hpp>
#include <rtEngine/mesh.hpp>

class FreeCam : public Component
{
    float speed = 5.0f, sensitivity = 0.15f;

    void Start() override
    {
        // game_object->translateGlobal(glm::vec3(-2.0f,2.0f,-0.5f));
    }

    void Update() override
    {

        if (game_object->my_engine.input.GetKeyDown(GLFW_KEY_W))
        {
            game_object->my_engine.m_renderer.resetCombinedFrames();
            game_object->translateGlobal(glm::normalize(glm::vec3(game_object->getGlobalForward().x, 0.0f, game_object->getGlobalForward().z)) * glm::vec3(Time::getDeltaTime() * speed));
        }

        if (game_object->my_engine.input.GetKeyDown(GLFW_KEY_S))
        {
            game_object->my_engine.m_renderer.resetCombinedFrames();
            game_object->translateGlobal(glm::normalize(glm::vec3(game_object->getGlobalForward().x, 0.0f, game_object->getGlobalForward().z)) * glm::vec3(Time::getDeltaTime() * -speed));
        }

        if (game_object->my_engine.input.GetKeyDown(GLFW_KEY_A))
        {
            game_object->my_engine.m_renderer.resetCombinedFrames();
            game_object->translateLocal(glm::vec3(Time::getDeltaTime() * -speed, 0.0f, 0.0f));
        }

        if (game_object->my_engine.input.GetKeyDown(GLFW_KEY_D))
        {
            game_object->my_engine.m_renderer.resetCombinedFrames();
            game_object->translateLocal(glm::vec3(Time::getDeltaTime() * speed, 0.0f, 0.0f));
        }

        if (game_object->my_engine.input.GetKeyDown(GLFW_KEY_SPACE))
        {
            game_object->my_engine.m_renderer.resetCombinedFrames();
            game_object->translateGlobal(glm::vec3(0.0f, Time::getDeltaTime() * speed, 0.0f));
        }

        if (game_object->my_engine.input.GetKeyDown(GLFW_KEY_LEFT_SHIFT))
        {
            game_object->my_engine.m_renderer.resetCombinedFrames();
            game_object->translateGlobal(glm::vec3(0.0f, -Time::getDeltaTime() * speed, 0.0f));
        }

        if (game_object->my_engine.input.GetKeyDown(GLFW_KEY_Q))
        {
            game_object->my_engine.m_renderer.resetCombinedFrames();
            game_object->rotateLocalEuler(Time::getDeltaTime() * glm::vec3(0.0f, 100.0f, 0.0f));
        }

        if (game_object->my_engine.input.GetKeyDown(GLFW_KEY_E))
        {
            game_object->my_engine.m_renderer.resetCombinedFrames();
            game_object->rotateLocalEuler(Time::getDeltaTime() * glm::vec3(0.0f, -100.0f, 0.0f));
        }

        if(game_object->my_engine.input.GetKeyDown(GLFW_KEY_ESCAPE))
        {
            game_object->my_engine.quit();
        }
    }
};

class Spin : public Component
{
    void Update() override
    {
        game_object->rotateLocalEuler(Time::getDeltaTime() * glm::vec3(0.0f, 100.0f, 0.0f));
    }
};

int main()
{
    Engine e;
    e.addScene("game");
    e.setCurrentScene("game");
    e.getCurrentScene()->setSkybox("resources/textures/rosendal_plains_2_4k.hdr");

    GameObject *dragon = new GameObject("Dragon", glm::vec3(-7.0f, 5.0f, -7.0f), glm::vec3(0.0f, -70.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),e);
    dragon->model = new Model("resources/models/dragon/dragon_scaled.obj");
    e.getCurrentScene()->addGameObject(dragon);

    // GameObject *shrek = new GameObject("Shrek", glm::vec3(7.0f, 0.0f, -8.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),e);
    // shrek->model = new Model("resources/models/shrek/shrek_scaled.obj");
    // e.getCurrentScene()->addGameObject(shrek);

    GameObject *crate = new GameObject("Wooden Crate", glm::vec3(2.0f, 1.0f, -4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),e);
    crate->model = new Model("resources/models/container/untitled.obj");
    e.getCurrentScene()->addGameObject(crate);

    GameObject *crate2 = new GameObject("Wooden Crate 2", glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 30.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),e);
    crate2->model = new Model("resources/models/container/untitled.obj");
    crate->addChild(crate2);

    // GameObject *scene = new GameObject("Scenery", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.05f, 0.05f, 0.05f),e);
    // scene->model = new Model("resources/models/sponza2/sponza.obj");
    // e.getCurrentScene()->addGameObject(scene);

    GameObject *ground_plane = new GameObject("Ground plane", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),e);
    ground_plane->model = new Model("resources/models/ground/TestPlane.obj");
    e.getCurrentScene()->addGameObject(ground_plane);

    GameObject *camera = new GameObject("Main Camera", glm::vec3(0.0f, 5.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),e);
    camera->addComponent(new Camera());
    camera->addComponent(new FreeCam());
    e.getCurrentScene()->addGameObject(camera);

    e.run();
    return 0;
}