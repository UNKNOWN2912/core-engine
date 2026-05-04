#version 450

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec3 aNormal;



layout(push_constant) uniform PushConstant
{
    mat4 model;
} pushConstant;

layout(binding = 0) uniform UniformData 
{
    mat4 projection;
    mat4 view;
    vec3 cameraPosition;
    vec3 cameraFront;
    float time;
} uniformData;

layout(location = 0) out OutData
{
    vec2 uv;
    vec3 normal;
    vec3 fragPos;
    float time;
} outData;

void main()
{
    outData.time = uniformData.time;
    outData.uv = aUv;
    outData.normal = aNormal;
    outData.fragPos = (pushConstant.model * vec4(aPosition, 1.0)).xyz;
    
    mat4 view = mat4(mat3(uniformData.view));
    gl_Position = uniformData.projection * view  * vec4(aPosition, 1.0);
}