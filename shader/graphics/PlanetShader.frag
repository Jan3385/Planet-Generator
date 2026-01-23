in vec3 Normal;
in vec3 FragPos;
in vec3 Pos;

out vec4 FragColor;

uniform vec3 viewPos;

uniform vec3 ambientColor;
uniform float ambientIntensity;

layout(std140, binding = 0) uniform palette {
    vec4 deepOcean;
    vec4 shallowOcean;
    vec4 sand;
    vec4 grass;
    vec4 rock;
    vec4 snow;
};

#get MAX_POINT_LIGHTS

#include "LightTypes.glsl"

uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform DirectionLight directionalLight;

#include "LightFunctions.glsl"

vec3 GetColorAtHeight(float height){
    vec4 color;

    if(height < 0.04f){
        color = mix(deepOcean, shallowOcean, height/0.04f);
    }
    else if(height < 0.05f) color = sand;
    else if(height < 0.09f) color = grass;
    else if(height < 0.12f) color = rock;
    else color = snow;

    return color.rgb;
}

float fMod(float num, float mod){
    while(num > mod){
        num -= mod;
    }
    return num;
}

void main()
{
    vec3 result = vec3(0.0f);
    vec3 viewDir = normalize(viewPos - FragPos);

    float height = length(Pos)-1;

    vec3 color = GetColorAtHeight(height);

    Material mat;
    mat.ambient = color;
    mat.diffuse = color;
    mat.specular = color;
    mat.shininess = 0.2f;

    // Point lights
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);
    for(int i = 0; i < numPointLights; i++) {
        result += CalculatePointLight(pointLights[i], Normal, FragPos, viewDir, mat);
    }

    // directional light
    result += CalculateDirLight(directionalLight, Normal, viewDir, mat);

    // ambient lighting
    result += (ambientColor * mat.ambient) * ambientIntensity;

    FragColor = vec4(result, 1.0f);
} 