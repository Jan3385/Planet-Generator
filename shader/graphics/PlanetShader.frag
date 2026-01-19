in vec3 Normal;
in vec3 FragPos;
in vec3 color;

out vec4 FragColor;

uniform vec3 viewPos;

uniform vec3 ambientColor;
uniform float ambientIntensity;

#get MAX_POINT_LIGHTS

#include "LightTypes.glsl"

uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform DirectionLight directionalLight;

#include "LightFunctions.glsl"

void main()
{
    vec3 result = vec3(0.0f);
    vec3 viewDir = normalize(viewPos - FragPos);

    Material mat;
    mat.ambient = color;
    mat.diffuse = color;
    mat.specular = color;
    mat.shininess = 0.2f;

    // Point lights
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);
    for(int i = 0; i < numPointLights; i++) {
        result += CalculatePointLight(pointLights[i], Normal, FragPos, viewDir, mat);
    }

    // directional light
    result += CalculateDirLight(directionalLight, Normal, viewDir, mat);

    // ambient lighting
    result += (ambientColor * mat.ambient) * ambientIntensity;

    FragColor = vec4(result, 1.0f);
} 