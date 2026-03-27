#include "ShaderOutputs.glsl"

in vec3 Normal;
in vec3 FragPos;

#include "LightTypes.glsl"

uniform Material material;

void main()
{
    gPosition = vec4(FragPos, 1.0);
    gNormal = vec4(normalize(Normal), 1.0);

    gAlbedo.rgb = material.color.rgb;

    gMetalRough.r = material.MetalRough.r;
    gMetalRough.g = material.MetalRough.g;
} 