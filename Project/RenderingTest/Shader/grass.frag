#version 450

layout(location = 0) out vec4 outputColor;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

layout(location = 5) in float time;

layout(location = 6) flat in int instanceIndex;

// Gradient function to generate random unit vectors
vec2 grad(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

// Perlin noise function
float perlinNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    // Quintic fade curve: 6t^5 - 15t^4 + 10t^3
    vec2 u = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);

    return mix(mix(dot(grad(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)),
                   dot(grad(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),
               mix(dot(grad(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)),
                   dot(grad(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x), u.y);
}


vec3 palette(float t, vec3 a, vec3 b, vec3 c, vec3 d )
{
    return a + b*cos( 6.283185*(c*t+d) );
}

vec3 colors[] = vec3[]
(
    vec3(0.847, 0.231, 0.514),
    vec3(0.102, 0.675, 0.388),
    vec3(0.961, 0.537, 0.114),
    vec3(0.243, 0.408, 0.882),
    vec3(0.718, 0.102, 0.102),
    vec3(0.055, 0.737, 0.820),
    vec3(0.580, 0.169, 0.824),
    vec3(0.929, 0.871, 0.173),
    vec3(0.173, 0.573, 0.173),
    vec3(0.961, 0.380, 0.255)
);

void main()
{
    vec3 gradiant1 = vec3(0.0078, 0.1294, 0.0078) * 0.1;
    vec3 gradiant2 = vec3(0.3843, 0.6352, 0.1372) * 0.4;

    gradiant2 = colors[instanceIndex % 10];

    vec3 color = mix(gradiant1, gradiant2, uv.y  + (perlinNoise(fragPos.xz) * 0.2));

    color = palette(1 - uv.y, vec3(0.470,0.470,-3.172),vec3(0.340,0.340,0.108),vec3(0.244,0.198,0.187),vec3(-0.705,-1.783,-1.523));

    outputColor = vec4(color * ((perlinNoise(fragPos.xz) + 4) / 5),1);

}