#version 450

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec3 aNormal;

layout(location = 0) out vec2 uv;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 fragPos;

layout(push_constant) uniform PushConstant
{
    mat4 model;
} pushConstant;

layout(binding = 0) uniform UniformData 
{
    mat4 projection;
    mat4 view;
    
} uniformData;

void main()
{
    uv = aUv;
    normal = aNormal;
    fragPos = (pushConstant.model * vec4(aPosition, 1.0)).xyz;
    mat4 view = mat4(mat3(uniformData.view));
    gl_Position = uniformData.projection * view  * vec4(aPosition, 1.0);
}