#version 460 core
out vec4 FragColor;
in vec2 TexCoord;

void main()
{
    vec3 unshaded = vec3(1,0,0);
    FragColor = vec4(unshaded, 1.0f);
}