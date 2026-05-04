#version 450

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 position;
layout(location = 2) out vec4 normal;

layout(set = 0, binding = 0) uniform sampler2D tex0;

layout(location = 0) in InputData
{
    vec3 fragPos;
    vec2 uv;
    vec3 normal;
} Input;

void main()
{
    albedo = texture(tex0, Input.uv);
    position = vec4(Input.fragPos, 1.0);
    normal = vec4(Input.normal,1);
}