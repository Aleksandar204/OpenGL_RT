#include<rtEngine/renderer.hpp>

int main()
{
    Renderer rend;
    while(!glfwWindowShouldClose(rend.m_window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        rend.renderFrame();
        
        glfwPollEvents();
        glfwSwapBuffers(rend.m_window);
    }
    return 0;
}