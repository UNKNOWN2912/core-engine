#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUv;

layout(location = 0) out vec2 uv;
layout(location = 1) out vec3 fragPos;

layout(binding = 0) uniform UniformData {
    
    mat4 model, view, projection;

} uniformData;




void main()
{
    fragPos = (uniformData.model * vec4(aPos, 1.0)).xyz;
    uv = aUv;
    gl_Position = uniformData.projection * uniformData.view * uniformData.model * vec4(aPos, 1.0);
}