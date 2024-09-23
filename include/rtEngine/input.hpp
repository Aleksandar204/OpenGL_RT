#pragma once
#include <glad/glad.h>

class Input
{
private:
    GLFWwindow* my_window;
public:
    bool GetKeyDown(int key_code)
    {
        return (glfwGetKey(my_window, key_code) == GLFW_PRESS);
    }
    Input(GLFWwindow* win){my_window = win;}
};