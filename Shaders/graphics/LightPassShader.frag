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

#var int SpecialRenderMode

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
        lighting += CalculatePointLight(pointLights[i], Normal, FragPos, viewDir, Albedo, 1-Specular);
    }

    // directional light
    lighting += CalculateDirLight(directionalLight, Normal, viewDir, Albedo, 1-Specular);

    if(SpecialRenderMode == 0)
        FragColor = vec4(lighting, 1.0);
    else if(SpecialRenderMode == 1)
        FragColor = vec4(Normal * 0.5 + 0.5, 1.0f);
    else if(SpecialRenderMode == 2)
        FragColor = vec4(Albedo, 1.0f);
    else if(SpecialRenderMode == 3)
        FragColor = vec4(Specular, Specular, Specular, 1.0f);
}