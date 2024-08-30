#include<rtEngine/engine.hpp>

#include<rtEngine/mesh.hpp>

int main()
{
    Engine e;
    e.addScene("game");
    e.setCurrentScene("game");
    e.run();
    return 0;
}