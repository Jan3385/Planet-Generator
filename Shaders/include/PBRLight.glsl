// ---- Definition of support functions ----

#include "LightTypes.glsl"

// ratio between reflection/refraction
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 viewDir, vec3 L, float roughness);

// ---- main functions ----
vec3 CalculatePointLightPBR(PointLightPBR light, vec3 Normal, vec3 FragPos, vec3 ViewDir, vec3 Albedo, float Metallic, float Roughness);
vec3 CalculateDirLightPBR(DirectionLightPBR light, vec3 Normal, vec3 ViewDir, vec3 Albedo, float Metallic, float Roughness);
float CalculateAttenuation(PointLightPBR light, float distance);

vec3 CalculatePointLightPBR(PointLightPBR light, vec3 Normal, vec3 FragPos, vec3 ViewDir, vec3 Albedo, float Metallic, float Roughness){
    vec3 lightDir = light.position - FragPos;

    vec3 L = normalize(lightDir);
    vec3 H = normalize(ViewDir + L);

    // attenuation
    float dist = length(lightDir);
    float attenuation = CalculateAttenuation(light, dist);
    vec3 radiance = light.light * attenuation;

    // cook tolerance specular BRDF
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, Metallic);
    vec3 F = fresnelSchlick(max(dot(H, ViewDir), 0.0), F0);

    float NDF = DistributionGGX(Normal, H, Roughness);
    float G = GeometrySmith(Normal, ViewDir, L, Roughness);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(Normal, ViewDir), 0.0) * max(dot(Normal, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    // diffuse component based on metallicity
    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - Metallic;

    // add radiance to total light
    float NdotL = max(dot(Normal, L), 0.0);
    return ((kD * Albedo / PI) + specular) * radiance * NdotL;
}

vec3 CalculateDirLightPBR(DirectionLightPBR light, vec3 Normal, vec3 ViewDir, vec3 Albedo, float Metallic, float Roughness){
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(ViewDir + L);

    // cook tolerance specular BRDF
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, Metallic);
    vec3 F = fresnelSchlick(max(dot(H, ViewDir), 0.0), F0);

    float NDF = DistributionGGX(Normal, H, Roughness);
    float G = GeometrySmith(Normal, ViewDir, L, Roughness);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(Normal, ViewDir), 0.0) * max(dot(Normal, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    // diffuse component based on metallicity
    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - Metallic;

    // add radiance to total light
    float NdotL = max(dot(Normal, L), 0.0);
    return ((kD * Albedo / PI) + specular) * light.light * NdotL;
}

float CalculateAttenuation(PointLightPBR light, float distance){
    return 1.0 / (
        light.constant + 
        light.linear * distance + 
        light.quadratic * (distance * distance)
    );
}

// ---- support functions ----
vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 viewDir, vec3 L, float roughness)
{
    float NdotV = max(dot(N, viewDir), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float GGX2  = GeometrySchlickGGX(NdotV, roughness);
    float GGX1  = GeometrySchlickGGX(NdotL, roughness);
	
    return GGX1 * GGX2;
}