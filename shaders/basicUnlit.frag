#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

void main()
{
    vec3 unshaded = vec3(1,0,0);
    FragColor = vec4(TexCoords, 0, 1.0f);
}