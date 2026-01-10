out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 ambientColor;
uniform float ambientIntensity;

void main()
{
    vec3 ambient = ambientColor * ambientIntensity;

    vec3 result = ambient * objectColor;
    FragColor = vec4(result, 1.0f);
} 