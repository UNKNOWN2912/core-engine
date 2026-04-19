#version 450

layout(location = 0) out vec4 outputColor;
layout(binding = 1) uniform sampler2D tex0;

vec3 lightDirection = vec3(1,1,1);

float rand(vec2 co) 
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

layout(location = 0) in InData
{
    vec3 fragPos;
    vec2 uv;
    vec3 normal;
    vec3 cameraPosition;
    vec3 cameraFront;
} inData;

void main()
{   
    lightDirection = normalize(lightDirection);

    float diffuse = max(dot(lightDirection, inData.normal), 0.0);
    float amdient = 0.2;



    vec3 color = texture(tex0, inData.uv).rgb;
    color = color * (diffuse + amdient);

    outputColor = vec4(color, 1.0);
}