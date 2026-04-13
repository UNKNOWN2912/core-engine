#version 450

layout(location = 0) in vec3 aPos;

layout(binding = 0) uniform UniformData 
{
    mat4 projection;
    mat4 view;
    mat4 model;
    
} uniformData;

void main()
{
    gl_Position = uniformData.projection * uniformData.view * uniformData.model * vec4(aPos, 1.0);
}