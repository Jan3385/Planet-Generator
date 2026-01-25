
in vec3 Normal;
in vec3 FragPos;

layout(std140, binding = 0) uniform palette {
    vec4 horizonColor;
    vec4 zenithColor;
};

uniform vec3 viewPos;

#include "LightTypes.glsl"
uniform DirectionLight directionalLight;

out vec4 FragColor;

#get LOW_POLY_FEEL

void main()
{
    #if LOW_POLY_FEEL == 1
        vec3 normalSample = normalize(Normal);
    #else
        vec3 normalSample = normalize(FragPos);
    #endif
    
    vec3 viewDir = normalize(viewPos - FragPos);

    float horizon = 1.0 - max(dot(viewDir, normalSample), 0.0);

    float sunAmount = max(dot(-directionalLight.direction, normalSample), 0.0) * 1.5f;

    float intensity = pow(horizon, 1.5f);
    intensity *= max(sunAmount, 0.5f);
    intensity = intensity * 0.6f;

    float t = smoothstep(0.0, 1.0, horizon);
    vec3 atmColor = mix(zenithColor.xyz, horizonColor.xyz, t);

    FragColor = vec4(atmColor * intensity, intensity);
} 