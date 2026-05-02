#version 450

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 position;
layout(location = 2) out vec4 normal;


layout(location = 0) in InData
{
    vec2 uv;
    vec3 normal;
    vec3 fragPos;
    float time;
} inData;


float f(float d)
{
    return sqrt(d);
}



void main()
{
    vec3 lightDirection = normalize(vec3(1,1,1));

    vec3 skyColor1 = vec3( 0.478, 0.718, 0.914);
    vec3 skyColor2 = vec3(0.02, 0.204, 0.384);
    vec3 color = skyColor2;

    vec3 pos = normalize(inData.fragPos);
    float d = abs(pos.y);

    color = vec3(d);
    color = mix(skyColor1, skyColor2, f(d));

    vec3 sunColor = vec3(1,1,1);

    float t = clamp(distance(pos, lightDirection), 0, 1);

    color += sunColor * pow(abs(1 - t), 4);

    albedo = vec4(color, 1.0);
}