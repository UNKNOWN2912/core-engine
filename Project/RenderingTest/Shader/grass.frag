#version 450

layout(location = 0) out vec4 outputColor;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

layout(location = 5) in float time;

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


void main()
{
    vec3 gradiant1 = vec3(0.0078, 0.1294, 0.0078) * 0.1;
    vec3 gradiant2 = vec3(0.3843, 0.6352, 0.1372) * 0.4;

    vec3 color = mix(gradiant1, gradiant2, uv.y  + (perlinNoise(fragPos.xz) * 0.2));

    outputColor = vec4(color,1);


}