in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 ambientColor;
uniform float ambientIntensity;

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
    vec3 diffuseSum = vec3(0.0f);
    for(int i = 0; i < numPointLights; i++) {
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = diff * pointLights[i].color * pointLights[i].intensity;
        diffuseSum += diffuse;
    }

    vec3 ambient = ambientColor * ambientIntensity;

    vec3 result = (ambient + diffuseSum) * objectColor;
    FragColor = vec4(result, 1.0f);
} 