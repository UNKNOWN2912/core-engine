#version 450

layout(location = 0) in vec3 aPosition;

layout(push_constant) uniform PushConstant
{
    mat4 model;
} pushConstant;

layout(std140, binding = 0) uniform UniformData 
{
    mat4 view;
    mat4 projection;
} uniformData;

void main()
{
    gl_Position = uniformData.projection * uniformData.view * pushConstant.model * vec4(aPosition, 1.0);
}