#version 450

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 position;
layout(location = 2) out vec4 normal;

layout(set = 0, binding = 0) uniform sampler2D tex0;

layout(location = 0) in InputData
{
    vec3 fragPos;
    vec2 uv;
    vec3 normal;
    vec3 worldCenter;
    vec3 cameraPosition;
    mat4 view;
    vec3 color;
} Input;

void main()
{
    vec2 coord = (Input.uv * 2.f) - 1.f;
    albedo = vec4(Input.color, 1);
    position = vec4(Input.fragPos, 1.0);
    normal = vec4(Input.normal,1);

    float len = dot(coord, coord);

    if(len > 1)
        discard;

    float z = sqrt(max(0, 1 - len));
    normal.rgb = vec3(coord, z);

    mat3 inverse = transpose(mat3(Input.view));
    normal.rgb = normalize(inverse * normal.rgb);

    vec3 lightDirection = vec3(1);

    float diffuse = max(dot(lightDirection, normal.rgb), 0);

    albedo.rgb *= diffuse;

    albedo = vec4(1,1,1,1);
}