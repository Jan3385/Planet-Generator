in vec3 Normal;
in vec3 FragPos;
in vec3 Pos;

out vec4 FragColor;

uniform vec3 viewPos;

#var vec3 ambientColor
#var float ambientIntensity

#include "LightTypes.glsl"

Material multMaterial(Material a, float factor) {
    a.ambient  *= factor;
    a.diffuse  *= factor;
    a.specular *= factor;
    return a;
}
Material mixMaterial(Material a, Material b, float t) {
    Material result;
    result.ambient  = mix(a.ambient,  b.ambient,  t);
    result.diffuse  = mix(a.diffuse,  b.diffuse,  t);
    result.specular = mix(a.specular, b.specular, t);
    result.shininess = mix(a.shininess, b.shininess, t);
    return result;
}

layout(std140, binding = 0) uniform palette {
    Material deepOcean;
    Material shallowOcean;
    Material sand;
    Material grass;
    Material rock;
    Material snow;
};

#get MAX_POINT_LIGHTS
#get PLANET_SCALE

uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform DirectionLight directionalLight;

#include "LightFunctions.glsl"
#include "Noise.glsl"

Material GetColorAtHeight(float height){
    Material mat;

    if(height < 0.013f * PLANET_SCALE){
        mat = mixMaterial(deepOcean, shallowOcean, height/(0.013f * PLANET_SCALE));
    }
    else if(height < 0.016f * PLANET_SCALE) mat = sand;
    else if(height < 0.040f * PLANET_SCALE){
        float t = valNoise(normalize(Pos) * 70.0f);
        mat = mixMaterial(multMaterial(grass, 0.95f), multMaterial(grass, 1.05f), t);

        float greenModifier = valNoise(normalize(Pos) * 2.0f);
        mat.ambient.g *= mix(0.8f, 1.2f, greenModifier);
        mat.diffuse.g *= mix(0.8f, 1.2f, greenModifier);
        mat.specular.g *= mix(0.8f, 1.2f, greenModifier);
    }
    else if(height < 0.052f * PLANET_SCALE) mat = rock;
    else mat = snow;


    return mat;
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

    Material mat = GetColorAtHeight(height);

    // Point lights
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);
    for(int i = 0; i < numPointLights; i++) {
        result += CalculatePointLight(pointLights[i], Normal, FragPos, viewDir, mat);
    }

    // directional light
    result += CalculateDirLight(directionalLight, Normal, viewDir, mat);

    // ambient lighting
    result += (ambientColor * mat.ambient.xyz) * ambientIntensity;

    FragColor = vec4(result, 1.0f);
} 