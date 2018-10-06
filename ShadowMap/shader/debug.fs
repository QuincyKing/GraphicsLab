#version  430 core

in vec2 Texcoords;
out vec4 FragColor;

uniform sampler2D depthMap;

void main()
{
    float color = texture(depthMap, Texcoords).r;
    FragColor = vec4(vec3(color), 1.0);
}