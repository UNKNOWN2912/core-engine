#version 450

layout(location = 0) out vec4 outputColor;
layout(location = 0) in vec2 uv;

layout(location = 2) in vec3 fragPos;

layout(binding = 1) uniform sampler2D tex0;

void main()
{
    float d = gl_FragCoord.z / gl_FragCoord.w;

    float fogFactor = clamp(exp(-0.020 * d), 0, 1);

    outputColor = texture(tex0, uv);
    // outputColor.a = fogFactor;
}