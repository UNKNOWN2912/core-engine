#version 450

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 position;
layout(location = 2) out vec4 normal;

layout(location = 0) in InputData
{
    vec3 fragPos;
} Input;

void main()
{
    albedo = vec4(0,1,1,1);
    position = vec4(Input.fragPos, 1.0);
    normal = vec4(1,1,0,1);
}