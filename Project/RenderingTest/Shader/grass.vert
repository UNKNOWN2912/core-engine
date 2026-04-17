#version 450

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec3 aNormal;

layout(location = 3) in mat4 instanceModelMatrix;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec2 uv;
layout(location = 2) out vec3 normal;

layout(binding = 0) uniform UniformData 
{
    mat4 projection;
    mat4 view;
    vec3 cameraPosition;
    float pad1;
    vec3 cameraFront;
    float pad2;
    float time;
    float pad[3];
    
} uniformData;

layout(location = 3) out vec3 cameraPosition;
layout(location = 4) out vec3 cameraFront;


layout(location = 5) out float time;

layout(location = 6) flat out int instanceIndex;


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
    instanceIndex = gl_InstanceIndex;
    cameraPosition = uniformData.cameraPosition;
    cameraFront = uniformData.cameraFront;

    time = uniformData.time;
    
    uv = aUv;
    normal = aNormal;
    fragPos = (instanceModelMatrix * vec4(aPosition, 1.0)).xyz;

    vec3 position = aPosition;
    float cameraDistance = distance(uniformData.cameraPosition, position);

    position.z += sqrt((1-uv.y) * 0.2);

    vec4 modelPosition = instanceModelMatrix * vec4(position, 1.0);
    float windDir = perlinNoise(modelPosition.xz * 0.09 +  0.5 * time) * 5;

    modelPosition.z += (windDir + clamp(1 - cameraDistance, 0, 1)) * pow(uv.y, 2);

    gl_Position = uniformData.projection * uniformData.view * modelPosition;

}