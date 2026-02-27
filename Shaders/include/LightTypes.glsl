struct MaterialOld {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 shininess;
};

struct Material {
    vec4 color;
    vec4 MetalRought; // .r = metallic, .g = roughness
};

struct PointLight {
    vec3 position;
    
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct PointLightPBR {
    vec3 position;
    vec3 light;

    float constant;
    float linear;
    float quadratic;
};

struct DirectionLight{
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
};

struct DirectionLightPBR{
    vec3 direction;
    vec3 light;
};