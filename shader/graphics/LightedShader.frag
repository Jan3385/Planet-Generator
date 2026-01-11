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
};
uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

void main()
{
    // diffuse lighting
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);
    for(int i = 0; i < numPointLights; i++) {
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);

        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128.0);

        diffuse += (diff * material.diffuse) * pointLights[i].diffuse;
        specular += (spec * material.specular) * pointLights[i].specular;
    }

    // ambient lighting
    vec3 ambient = (ambientColor * material.ambient) * ambientIntensity;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
} 