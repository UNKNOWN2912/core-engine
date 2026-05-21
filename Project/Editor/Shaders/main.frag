#version 450
layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 position;
layout(location = 2) out vec4 normal;

void main()
{
    albedo = vec4(1.0, 0.0, 0.0, 1.0);
    position = vec4(0.0, 1.0, 0.0, 1.0);
    normal = vec4(0.0, 0.0, 1.0, 1.0);
}