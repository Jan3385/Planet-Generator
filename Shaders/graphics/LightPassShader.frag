in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetalRough;
uniform sampler2D shadowMap;

#include "LightTypes.glsl"

#get PI

#get MAX_POINT_LIGHTS
uniform int numPointLights;
uniform PointLightPBR pointLights[MAX_POINT_LIGHTS];
uniform DirectionLightPBR directionalLight;

#var mat4 dirLightSpaceMatrix

#var vec3 ambientColor
#var float ambientIntensity

#var vec3 viewPos

#var int SpecialRenderMode

#include "PBRLight.glsl"

out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 Normal, vec3 lightDir){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    const float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005); 
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedo, TexCoords).rgb;

    vec4 fragPosLightSpace = dirLightSpaceMatrix * vec4(FragPos, 1.0);

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

    if(ShadowCalculation(fragPosLightSpace, Normal, directionalLight.direction) < 1.0){
        // directional light
        light += CalculateDirLightPBR(directionalLight, Normal, viewDir, Albedo, metallic, roughness);
    }

    // ambient
    vec3 ambient = ambientColor * ambientIntensity * Albedo * ao;

    vec3 color = ambient + light;

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
    else if(SpecialRenderMode == 5){
        float shadow = ShadowCalculation(fragPosLightSpace, Normal, directionalLight.direction);
        FragColor = vec4(vec3(shadow), 1.0f);
    }
    else if(SpecialRenderMode == 6) {
        // Visualize shadow map sampled depth
        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        projCoords = projCoords * 0.5 + 0.5;
        float sampledDepth = texture(shadowMap, projCoords.xy).r;
        FragColor = vec4(vec3(sampledDepth), 1.0f);
    }
    else if(SpecialRenderMode == 7) {
        // Visualize calculated fragment depth in light space
        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        projCoords = projCoords * 0.5 + 0.5;
        float currentDepth = projCoords.z;
        FragColor = vec4(vec3(currentDepth), 1.0f);
    }
}