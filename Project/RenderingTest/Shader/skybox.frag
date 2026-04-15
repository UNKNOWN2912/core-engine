#version 450

layout(location = 0) out vec4 outputColor;
layout(location = 0) in vec2 uv;

layout(location = 2) in vec3 fragPos;

const int count = 1000;

void main()
{
    vec3 gradient1 = vec3(0.8941, 0.4275, 0.2196);
    vec3 gradient2 = vec3(0.3922, 0.4157, 0.8275);

    vec3 color = mix(gradient1, gradient2, fragPos.y);
    outputColor = vec4(color,1);
}