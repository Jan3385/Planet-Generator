in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 viewPos;

#include "LightTypes.glsl"

uniform Material material;

uniform vec3 ambientColor;
uniform float ambientIntensity;

#get MAX_POINT_LIGHTS

uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform DirectionLight directionalLight;

#get LOW_POLY_FEEL

#include "LightFunctions.glsl"

void main()
{
    vec3 result = vec3(0.0f);
    vec3 viewDir = normalize(viewPos - FragPos);

    #if LOW_POLY_FEEL == 1
        vec3 normalSample = normalize(Normal);
    #else
        vec3 normalSample = normalize(FragPos);
    #endif

    // Point lights
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);
    for(int i = 0; i < numPointLights; i++) {
        result += CalculatePointLight(pointLights[i], normalSample, FragPos, viewDir, material);
    }

    // directional light
    result += CalculateDirLight(directionalLight, normalSample, viewDir, material);

    // ambient lighting
    result += (ambientColor * material.ambient.xyz) * ambientIntensity;

    FragColor = vec4(result, 1.0f);
} 