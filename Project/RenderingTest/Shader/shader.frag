#version 450

layout(location = 0) out vec4 outputColor;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

vec3 lightDirection = vec3(1,1,1);

void main()
{   
    lightDirection = normalize(lightDirection);

    float diffuse = max(dot(lightDirection, normal), 0.0);

    outputColor = vec4(vec3(0.0078, 0.1294, 0.0078) * 0.1 * diffuse, 1);

    float d = gl_FragCoord.z / gl_FragCoord.w;

}