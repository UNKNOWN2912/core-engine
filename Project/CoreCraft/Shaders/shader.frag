#version 450

layout(location = 0) out vec4 outputColor;
layout(binding = 1) uniform sampler2D tex0;
layout(binding = 3) uniform sampler2D shadowMap;

vec3 lightDirection = vec3(1,1,1);

float rand(vec2 co) 
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

layout(location = 0) in InData
{
    vec3 fragPos;
    vec4 lightFragCoord;
    vec2 uv;
    vec3 normal;
    vec3 cameraPosition;
    vec3 cameraFront;
    float time;
} inData;

vec3 colors[4] = 
{
    vec3(1,0,0),
    vec3(0,1,0),
    vec3(0,0,1),
    vec3(1,1,0),
};

void main()
{   
    vec3 lightCoord = inData.lightFragCoord.xyz / inData.lightFragCoord.w;
    // lightCoord.y *= -1;

    lightCoord.x = (lightCoord.x + 1.0) / 2.0;
    lightCoord.y = (lightCoord.y + 1.0) / 2.0;

    float shadow = 0.f;

    if(lightCoord.z < 1.0)
    {
        // float closestDepth = texture(shadowMap, lightCoord.xy).r;
        float currentDepth = lightCoord.z;

        float bias = 0.0005;//max(0.005 * (1.0 - dot(inData.normal, lightDirection)), 0.005);

        // if(currentDepth < closestDepth + bias)
        // {
        //     shadow = 1.0;
        // }

        int sampleRadius = 3;
        vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
        for (int y = -sampleRadius; y <= sampleRadius; y++)
        {
            for (int x = -sampleRadius; x <= sampleRadius; x++)
            {
                float closestDepth = texture(shadowMap, lightCoord.xy + vec2(x,y) * pixelSize).r;
                if(currentDepth > closestDepth + bias)
                {
                    shadow += 1.0;
                }
            }
        }

        shadow /= pow((sampleRadius * 2 + 1), 2);
    }

    float diffuse = max(dot(lightDirection, inData.normal), 0.0) * (1 - shadow);
    float amdient = 0.2;

    vec3 color = texture(tex0, inData.uv).rgb;
    color = color * (diffuse + amdient);


    outputColor = vec4(color * (colors[int(gl_FragCoord.z * 4)] * 0.5), 1.0);

    // outputColor = vec4(vec3(color), 1);
}