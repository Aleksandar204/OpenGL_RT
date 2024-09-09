#version 460 core

#extension GL_ARB_bindless_texture : require

out vec4 FragColor;
in vec2 TexCoords;

layout(bindless_sampler) uniform sampler2D albedo_texture_handle;

void main()
{
    vec3 unshaded = texture(albedo_texture_handle, TexCoords).rgb;
    FragColor = vec4(unshaded, 1.0f);
}