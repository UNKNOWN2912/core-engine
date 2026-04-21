#version 450

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec3 aNormal;


layout(push_constant) uniform PushConstant
{
    mat4 model;
} pushConstant;

layout(std140, binding = 0) uniform UniformData 
{
    mat4 projection;
    mat4 view;
    vec3 cameraPosition;
    vec3 cameraFront;
    float time;
} uniformData;

layout(std140, binding = 2) uniform LightUniformData 
{
    mat4 view;
    mat4 projection;
} lightUniformData;



layout(location = 0) out OutData
{
    vec3 fragPos;
    vec4 lightFragCoord;
    vec2 uv;
    vec3 normal;
    vec3 cameraPosition;
    vec3 cameraFront;
    float time;
} outData;


void main()
{
    outData.cameraPosition = uniformData.cameraPosition;
    outData.cameraFront = uniformData.cameraFront;
    outData.uv = aUv;
    outData.normal = aNormal;
    outData.fragPos = (pushConstant.model * vec4(aPosition, 1.0)).xyz;
    outData.lightFragCoord = lightUniformData.projection * lightUniformData.view * pushConstant.model * vec4(aPosition, 1.0);
    outData.time = uniformData.time;


    gl_Position = uniformData.projection * uniformData.view * pushConstant.model * vec4(aPosition, 1.0);
}