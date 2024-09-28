#version 460 core

#extension GL_ARB_bindless_texture : require

out vec4 FragColor;
in vec2 TexCoords;

layout(bindless_sampler) uniform sampler2D albedo_texture_handle;
uniform int sample_albedo_texture;

void main()
{
    vec3 unshaded;
    if(sample_albedo_texture == 1)
        unshaded = texture(albedo_texture_handle, TexCoords).rgb;
    else
        unshaded = vec3(TexCoords, 0.7f);
    FragColor = vec4(unshaded, 1.0f);
}