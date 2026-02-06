#include "ShaderOutputs.glsl"

in vec3 Normal;
in vec3 FragPos;

#include "LightTypes.glsl"

uniform Material material;

#get LOW_POLY_FEEL

void main()
{
    #if LOW_POLY_FEEL == 1
        vec3 normalSample = normalize(Normal);
    #else
        vec3 normalSample = normalize(FragPos);
    #endif

    gPosition = vec4(FragPos, 1.0);
    gNormal = vec4(normalSample, 1.0);

    gAlbedoSpec.rgb = material.diffuse.rgb;
    gAlbedoSpec.a = material.specular.r;
} 