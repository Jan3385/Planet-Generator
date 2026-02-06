in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

#include "LightTypes.glsl"

#get MAX_POINT_LIGHTS
uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform DirectionLight directionalLight;

#var vec3 ambientColor
#var float ambientIntensity

#var vec3 viewPos

#include "LightFunctions.glsl"

out vec4 FragColor;

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    // ambient
    vec3 lighting = ambientColor * ambientIntensity * Albedo;

    // point lights
    vec3 viewDir = normalize(viewPos - FragPos);
    for(int i = 0; i < numPointLights; i++){
        lighting += CalculatePointLight(pointLights[i], Normal, FragPos, viewDir, Albedo, Specular);
    }

    // directional light
    lighting += CalculateDirLight(directionalLight, Normal, viewDir, Albedo, Specular);

    //FragColor = vec4(Normal * 0.5 + 0.5, 1.0f);
    FragColor = vec4(lighting, 1.0);
}