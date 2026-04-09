#version 450

layout(location = 0) out vec4 outputColor;

layout(binding = 1) uniform sampler2D tex0;

layout(location = 0) in vec2 uv;


void main()
{
    outputColor = texture(tex0, uv);
}