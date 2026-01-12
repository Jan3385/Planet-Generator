struct PointLight {
    vec3 position;
    
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirectionLight{
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
};