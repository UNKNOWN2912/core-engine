#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(set = 1, binding = 0) uniform sampler2D albedoTextures[];

layout(location = 0) in InputData
{
    vec3 fragPos;
    vec3 cameraPos;
    vec2 uv;
}
Input;

layout(push_constant) uniform PushConstant
{
    mat4 model;
    uint albedoIndex;
    uint specularIndex;
    uint roughnessIndex;
    uint metallicIndex;
}
pushConstant;

void main()
{
    if (texture(albedoTextures[pushConstant.albedoIndex], Input.uv).a < 0.1)
    {
        discard;
    }

    gl_FragDepth = distance(Input.cameraPos, Input.fragPos) / 100.f;
}