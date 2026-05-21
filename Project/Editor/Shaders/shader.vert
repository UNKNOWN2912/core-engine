#version 450
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec3 aNormal;

layout(location = 0) out OutputData
{
    vec3 fragPos;
    vec2 uv;
    vec3 normal;
} Output;

layout(set = 1, binding = 0) uniform UniformData
{
    mat4 projection;
    mat4 view;
    vec3 cameraPosition;
} data;

layout(push_constant) uniform PushConstantData
{
    mat4 model;
} pushContantData;

void main()
{
    Output.fragPos = vec3(pushContantData.model * vec4(aPosition, 1.0));
    Output.uv = aUv;
    Output.normal = aNormal;
    
    gl_Position = data.projection * data.view * pushContantData.model * vec4(aPosition, 1.0);
}