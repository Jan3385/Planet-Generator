#include "LightTypes.glsl"

vec3 CalculateDirLight(DirectionLight light, vec3 normal, vec3 viewDir, MaterialOld mat){
    vec3 dirLightDir = normalize(-light.direction);
    vec3 reflectDir = reflect(-dirLightDir, normal);

    float dirDiff = max(dot(normal, dirLightDir), 0.0);
    float dirSpec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess.x * 128.0);

    vec3 diffuse = (dirDiff * mat.diffuse.xyz) * light.diffuse.xyz;
    vec3 specular = (dirSpec * mat.specular.xyz) * light.specular.xyz;

    return (diffuse + specular);
}
vec3 CalculateDirLight(DirectionLight light, vec3 normal, vec3 viewDir, vec3 albedo, float shininess){
    vec3 dirLightDir = normalize(-light.direction);
    vec3 reflectDir = reflect(-dirLightDir, normal);

    float dirDiff = max(dot(normal, dirLightDir), 0.0);
    float dirSpec = pow(max(dot(viewDir, reflectDir), 0.0), shininess * 128.0);

    vec3 diffuse = (dirDiff * albedo) * light.diffuse.xyz;
    vec3 specular = (dirSpec) * light.specular.xyz;

    return (diffuse + specular);
}

float CalculateAttenuation(PointLight light, vec3 fragPos){
    float distance = length(light.position - fragPos);
    return 1.0 / (
        light.constant + 
        light.linear * distance + 
        light.quadratic * (distance * distance)
    );
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, MaterialOld mat){
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfWayDir = normalize(lightDir + viewDir);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfWayDir), 0.0), mat.shininess.x * 128.0);

    float attenuation = CalculateAttenuation(light, fragPos);

    vec3 diffuse = (diff * mat.diffuse.xyz) * light.diffuse.xyz * attenuation;
    vec3 specular = (spec * mat.specular.xyz) * light.specular.xyz * attenuation;

    return (diffuse + specular);
}
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float shininess){
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfWayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(normal, halfWayDir), 0.0), shininess * 128.0);

    float attenuation = CalculateAttenuation(light, fragPos);

    vec3 diffuse = (diff * albedo) * light.diffuse.xyz * attenuation;
    vec3 specular = (spec) * light.specular.xyz * attenuation;

    return (diffuse + specular);
}