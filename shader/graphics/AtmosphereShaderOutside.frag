
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

#define HORIZON_INTENSITY_EXPONENT 0.25f
#define SUN_INTENSITY 1.5f
#define CAMERA_CLEAR_DISTANCE_FACTOR 2.0f

void main()
{
    #if LOW_POLY_FEEL == 1
        vec3 normalSample = normalize(Normal);
    #else
        vec3 normalSample = normalize(FragPos);
    #endif
    
    vec3 viewDir = normalize(viewPos - FragPos);

    // color intensity based on horizon
    float horizon = 1.0 - max(dot(viewDir, normalSample), 0.0);
    float intensity = pow(horizon, HORIZON_INTENSITY_EXPONENT);

    // color intensity based on sun orientation
    float sunAmount = max(dot(-directionalLight.direction, normalSample), 0.0) * SUN_INTENSITY;
    intensity *= max(sunAmount, 0.5f);

    intensity = intensity * 0.6f;

    // color fade out based on distance from camera
    float distanceFromPixel = length(viewPos - FragPos);
    intensity *= clamp(mix(0.0f, 1.0f, distanceFromPixel * CAMERA_CLEAR_DISTANCE_FACTOR), 0, 1.0f);

    // final color mix
    float t = smoothstep(0.0, 1.0, horizon);
    vec3 atmColor = mix(zenithColor.xyz, horizonColor.xyz, t);

    FragColor = vec4(atmColor * intensity, intensity);
} 