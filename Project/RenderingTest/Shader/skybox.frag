#version 450

layout(location = 0) out vec4 outputColor;
layout(location = 0) in vec2 uv;

layout(location = 2) in vec3 fragPos;

float rand(vec2 co) 
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{

    vec3 lightDirection = normalize(vec3(1,1,1));
    vec3 pos = normalize(fragPos);

    vec3 sunColor = vec3(0.8,0.8,0.8);
    vec3 skyColor = vec3(0.1254, 0.6431, 0.8313);

    float d = clamp((distance(pos, lightDirection)), 0.0, 1.0);

    vec3 color = skyColor;

    color += sunColor * pow((1 - d), 4);

    outputColor = vec4(color, 1.0);

    outputColor.rgb = vec3(0.1);
}