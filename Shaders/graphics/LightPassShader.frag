in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetalRough;

#include "LightTypes.glsl"

#get PI

#get MAX_POINT_LIGHTS
uniform int numPointLights;
uniform PointLightPBR pointLights[MAX_POINT_LIGHTS];
uniform DirectionLightPBR directionalLight;

#var vec3 ambientColor
#var float ambientIntensity

#var vec3 viewPos

#var int SpecialRenderMode

#include "PBRLight.glsl"

out vec4 FragColor;

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedo, TexCoords).rgb;

    vec2 MetalRough = texture(gMetalRough, TexCoords).rg;
    float metallic = MetalRough.x;
    float roughness = MetalRough.y;

    float ao = 1.0; //TODO: add SSAO

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 light = vec3(0.0);

    // point lights
    for(int i = 0; i < numPointLights; ++i){
        light += CalculatePointLightPBR(pointLights[i], Normal, FragPos, viewDir, Albedo, metallic, roughness);
    }

    // directional light
    light += CalculateDirLightPBR(directionalLight, Normal, viewDir, Albedo, metallic, roughness);

    // ambient
    vec3 ambient = ambientColor * ambientIntensity * Albedo * ao;

    vec3 color = ambient + light;

    // point lights
    //for(int i = 0; i < numPointLights; i++){
    //    lighting += CalculatePointLight(pointLights[i], Normal, FragPos, viewDir, Albedo, 1-Specular);
    //}

    // directional light
    //lighting += CalculateDirLight(directionalLight, Normal, viewDir, Albedo, 1-Specular);

    if(SpecialRenderMode == 0)
        FragColor = vec4(color, 1.0);
    else if(SpecialRenderMode == 1)
        FragColor = vec4(Normal * 0.5 + 0.5, 1.0f);
    else if(SpecialRenderMode == 2)
        FragColor = vec4(Albedo, 1.0f);
    else if(SpecialRenderMode == 3)
        FragColor = vec4(metallic, 0.0f, 0.0f, 1.0f);
    else if(SpecialRenderMode == 4)
        FragColor = vec4(roughness, 0.0f, 0.0f, 1.0f);
}