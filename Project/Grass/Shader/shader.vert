#version 450

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec3 aNormal;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec2 uv;
layout(location = 2) out vec3 normal;

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
    
} uniformData;

layout(location = 3) out vec3 cameraPosition;
layout(location = 4) out vec3 cameraFront;


void main()
{
    cameraPosition = uniformData.cameraPosition;
    cameraFront = uniformData.cameraFront;
    
    uv = aUv;
    normal = aNormal;
    fragPos = (pushConstant.model * vec4(aPosition, 1.0)).xyz;

    gl_Position = uniformData.projection * uniformData.view * pushConstant.model * vec4(aPosition, 1.0);
}