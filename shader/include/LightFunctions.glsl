#include "LightTypes.glsl"

vec3 CalculateDirLight(DirectionLight light, vec3 normal, vec3 viewDir){
    vec3 dirLightDir = normalize(-light.direction);
    vec3 reflectDir = reflect(-dirLightDir, normal);

    float dirDiff = max(dot(normal, dirLightDir), 0.0);
    float dirSpec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128.0);

    vec3 diffuse = (dirDiff * material.diffuse) * light.diffuse;
    vec3 specular = (dirSpec * material.specular) * light.specular;

    return (diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128.0);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (
        light.constant + 
        light.linear * distance + 
        light.quadratic * (distance * distance)
    );

    vec3 diffuse = (diff * material.diffuse) * light.diffuse * attenuation;
    vec3 specular = (spec * material.specular) * light.specular * attenuation;

    return (diffuse + specular);
}