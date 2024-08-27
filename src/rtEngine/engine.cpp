#include <rtEngine/engine.hpp>

void Engine::run()
{
    while(!glfwWindowShouldClose(m_renderer.m_window))
    {
        m_renderer.renderFrame();
        glfwPollEvents();
    }
}