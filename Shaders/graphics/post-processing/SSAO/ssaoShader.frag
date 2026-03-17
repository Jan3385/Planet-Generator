in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

#define KERNEL_SIZE 64
uniform vec3 samples[KERNEL_SIZE];

#var mat4 projection

#var vec2 screenSize
const vec2 noiseScale = screenSize / 4.0;

out float FragColor;

void main(){
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    const float radius = 1.0;
    const float bias = 0.025;

    float occlusion = 0.0;
    for(int i = 0; i < KERNEL_SIZE; ++i){
        // get sample position
        vec3 sampleVec = TBN * samples[i];
        vec3 samplePos = fragPos + sampleVec * radius;

        float sampleDepth = texture(gPosition, TexCoords + sampleVec.xy * radius).z;

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth) + 0.0001);
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / KERNEL_SIZE);
    FragColor = occlusion;
}

