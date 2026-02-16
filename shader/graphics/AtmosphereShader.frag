
in vec3 Normal;
in vec3 FragPos;

layout(std140, binding = 0) uniform palette {
    vec4 horizonColor;
    vec4 zenithColor;
};
uniform vec3 originPos;

#include "LightTypes.glsl"
uniform DirectionLight directionalLight;

#var vec3 viewPos

#get LOW_POLY_FEEL

#define HORIZON_INTENSITY_EXPONENT 0.8f
#define SUN_INTENSITY 1.5f
#define CAMERA_CLEAR_DISTANCE_FACTOR 2.0f

out vec4 FragColor;

#include "Color.glsl"

void main()
{
    #if LOW_POLY_FEEL == 1
        vec3 normalSample = normalize(Normal);
    #else
        vec3 normalSample = normalize(FragPos - originPos);
    #endif
    
    vec3 viewDir = normalize(viewPos - FragPos);

    // color intensity based on horizon
    float horizon = abs(dot(viewDir, normalSample)) * 2.2f;
    //float intensity = pow(horizon, HORIZON_INTENSITY_EXPONENT);
    float intensity = 1.0f;

    // color intensity based on sun orientation
    float sunAmount = max(dot(-directionalLight.direction, normalSample), 0.0) * SUN_INTENSITY;
    intensity *= max(sunAmount, 0.5f);

    intensity = intensity * 0.6f;

    // color fade out based on distance from camera
    float distanceFromPixel = length(viewPos - FragPos);
    intensity *= clamp(mix(0.0f, 1.0f, distanceFromPixel*CAMERA_CLEAR_DISTANCE_FACTOR), 0, 1.0f);

    // final color mix
    float t = smoothstep(0.0, 1.0, horizon);
    vec3 atmColor = mix(zenithColor.xyz, horizonColor.xyz, t);

    FragColor = vec4(toLinear(atmColor * intensity), intensity);
} 