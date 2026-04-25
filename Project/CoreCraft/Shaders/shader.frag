#version 450

layout(location = 0) out vec4 outputColor;
layout(binding = 1) uniform sampler2D tex0;

layout(location = 0) in InData
{
    vec3 fragPos;
    vec2 uv;
    vec3 normal;
    vec3 cameraPosition;
    vec3 cameraFront;
    float time;
} inData;



void main()
{   
    vec3 lightDirection = normalize(vec3(1));

    float diffuse = max(dot(lightDirection, inData.normal), 0.0);
    float ambient = 0.1;

    outputColor = texture(tex0, inData.uv);
    outputColor.rgb *= (diffuse + ambient);
}