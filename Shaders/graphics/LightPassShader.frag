in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetalRough;

uniform sampler2D dlShadowMap;

#include "LightTypes.glsl"

#get PI

#get plFarPlane

#get MAX_POINT_LIGHTS
uniform int numPointLights;
uniform PointLightPBR pointLights[MAX_POINT_LIGHTS];
uniform samplerCube plShadowMap[MAX_POINT_LIGHTS];
uniform DirectionLightPBR directionalLight;

#var mat4 dirLightSpaceMatrix

#var vec3 ambientColor
#var float ambientIntensity

#var vec3 viewPos

#var int SpecialRenderMode

#include "PBRLight.glsl"

out vec4 FragColor;

float dlShadowCalculation(vec4 fragPosLightSpace, vec3 Normal, vec3 lightDir){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) return 0.0;
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(dlShadowMap, 0);

    const float bias = max(0.002 * (1.0 - dot(normalize(Normal), normalize(lightDir))), 0.005);
    //const float bias = 0.005;
    
    float currentDepth = projCoords.z;

    for(int x = -1; x <= 1; ++x){
        for(int y = -1; y <= 1; ++y){
            float pcfDepth = texture(dlShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow = shadow / 9.0;
    
    return shadow;
}

float plShadowCalculation(vec3 fragPos, float farPlane, vec3 lightPos, samplerCube plShadowMap){
    vec3 fragToLight = fragPos - lightPos;

    float currentDepth = length(fragToLight);

    float bias = 0.05; 
    float shadow = 0.0;

    float samples = 4.0;
    float offset  = 0.1;
    for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                float pfcDepth = texture(plShadowMap, fragToLight + vec3(x, y, z)).r;
                pfcDepth *= farPlane;
                shadow += currentDepth - bias > pfcDepth ? 1.0 : 0.0;
            }
        }
    }

    shadow /= (samples * samples * samples);

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
        float shadow = plShadowCalculation(FragPos, plFarPlane, pointLights[i].position, plShadowMap[i]);
        light += CalculatePointLightPBR(pointLights[i], Normal, FragPos, viewDir, Albedo, metallic, roughness) * (1 - shadow);
    }

    // directional light
    vec3 dirLight = CalculateDirLightPBR(directionalLight, Normal, viewDir, Albedo, metallic, roughness);
    dirLight *= 1 - dlShadowCalculation(fragPosLightSpace, Normal, directionalLight.direction);
    light += dirLight;

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
        float shadow = dlShadowCalculation(fragPosLightSpace, Normal, directionalLight.direction);
        FragColor = vec4(vec3(shadow), 1.0f);
    }
}