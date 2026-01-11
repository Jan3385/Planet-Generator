in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

uniform vec3 ambientColor;
uniform float ambientIntensity;

#get MAX_POINT_LIGHTS

struct PointLight {
    vec3 position;
    
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

struct DirectionLight{
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionLight directionalLight;

void main()
{
    // diffuse + specular lighting
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);
    for(int i = 0; i < numPointLights; i++) {
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);

        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128.0);

        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (
            pointLights[i].constant + 
            pointLights[i].linear * distance + 
            pointLights[i].quadratic * (distance * distance)
        );

        diffuse += (diff * material.diffuse) * pointLights[i].diffuse * attenuation;
        specular += (spec * material.specular) * pointLights[i].specular * attenuation;
    }

    // directional light
    vec3 dirLightDir = normalize(-directionalLight.direction);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-dirLightDir, Normal);

    float dirDiff = max(dot(Normal, dirLightDir), 0.0);
    float dirSpec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128.0);
    diffuse += (dirDiff * material.diffuse) * directionalLight.diffuse;
    specular += (dirSpec * material.specular) * directionalLight.specular;

    // ambient lighting
    vec3 ambient = (ambientColor * material.ambient) * ambientIntensity;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
} 