in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 viewPos;

uniform vec3 objectColor;
uniform vec3 ambientColor;
uniform float ambientIntensity;
uniform float specularIntensity;

#get MAX_POINT_LIGHTS

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};
uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

void main()
{
    const float shininess = 32.0f;

    // diffuse lighting
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);
    for(int i = 0; i < numPointLights; i++) {
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);

        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

        diffuse += diff * pointLights[i].color * pointLights[i].intensity;
        specular += specularIntensity * spec * pointLights[i].color * pointLights[i].intensity;
    }

    // ambient lighting
    vec3 ambient = ambientColor * ambientIntensity;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0f);
} 