#version 450

layout(location = 0) out vec4 outputColor;
layout(location = 0) in vec2 uv;

layout(location = 2) in vec3 fragPos;

layout(location = 3) in float time;

float rand(vec2 co) 
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 palette( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    return a + b*cos( 6.283185*(c*t+d) );
}

vec3 setPalette(float t)
{
    return palette(t, vec3(0.5, 0.5, 0.5), vec3(0.5, 0.5, 0.5), vec3(1.0, 1.0, 1.0), vec3(0.00, 0.33, 0.67));
}

float f(float d)
{
    return sqrt(d);
}

void main()
{
    vec3 lightDirection = normalize(vec3(1));

    vec3 skyColor1 = vec3( 0.478, 0.718, 0.914);
    vec3 skyColor2 = vec3(0.02, 0.204, 0.384);
    vec3 color = skyColor2;

    vec3 pos = normalize(fragPos);
    float d = abs(pos.y);

    color = vec3(d);
    color = mix(skyColor1, skyColor2, f(d));

    vec3 sunColor = vec3(1,1,1);

    float t = clamp(distance(pos, lightDirection), 0, 1);

    color += sunColor * pow(abs(1 - t), 4);

    outputColor = vec4(color, 1.0);
}