#version 450
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aParticlePositions;

layout(location = 0) out OutputData
{
    vec3 fragPos;
    vec2 uv;
    vec3 normal;
    vec3 worldCenter;
    vec3 cameraPosition;
    mat4 view;
    vec3 color;
} Output;

layout(set = 1, binding = 0) uniform UniformData
{
    mat4 projection;
    mat4 view;
    vec3 cameraPosition;
} data;

vec3 hash3(float n) 
{
    return fract(sin(vec3(n, n + 1.0, n + 2.0)) * 43758.5453123);
}

void main()
{
    vec3 position = aPosition + aParticlePositions;
    
    Output.color = hash3(float(gl_InstanceIndex));

    Output.fragPos = position;
    Output.uv = aUv;
    Output.normal = aNormal;
    Output.worldCenter = aParticlePositions;
    Output.cameraPosition = data.cameraPosition;
    Output.view = data.view;

    mat4 view = data.view;
    vec3 t = aParticlePositions;
    mat4 model = mat4(
        1  ,0  ,0  ,  0,
        0  ,1  ,0  ,  0,
        0  ,0  ,1  ,  0,
        t.x,t.y,t.z,  1
    );

    mat4 modelView = view * model;

    modelView[0][0] = 1;
    modelView[0][1] = 0;
    modelView[0][2] = 0;

    modelView[1][0] = 0;
    modelView[1][1] = 1;
    modelView[1][2] = 0;

    modelView[2][0] = 0;
    modelView[2][1] = 0;
    modelView[2][2] = 1;
    
    gl_Position = data.projection * modelView * vec4(aPosition, 1.0);
}