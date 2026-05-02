#version 450
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec3 aNormal;

layout(location = 0) out OutputData
{
    vec3 fragPos;
} Output;

void main()
{
    Output.fragPos = aPosition;
    
    gl_Position = vec4(aPosition, 1.0);
}