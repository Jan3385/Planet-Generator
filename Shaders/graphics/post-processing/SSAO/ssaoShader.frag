in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

#define KERNEL_SIZE 64
uniform vec3 samples[KERNEL_SIZE];

#var mat4 projection
#var mat4 view

#var vec2 screenSize
const vec2 noiseScale = screenSize / 4.0;

out float FragColor;

void main(){
    vec3 worldPos    = texture(gPosition, TexCoords).xyz;
    vec3 worldNormal = texture(gNormal,   TexCoords).xyz;

    // Convert to view-space once
    vec3 fragPos = (view * vec4(worldPos, 1.0)).xyz;
    vec3 normal  = normalize(mat3(view) * worldNormal);

    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz * 2.0 - 1.0);

    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);

    const float radius = 1.5;
    const float bias   = 0.025;

    float occlusion = 0.0;
    for(int i = 0; i < KERNEL_SIZE; ++i){
        vec3 samplePos = fragPos + (TBN * samples[i]) * radius;

        vec4 offset = projection * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;
        offset.xyz  = offset.xyz * 0.5 + 0.5;

        vec3 fetchedWorld = texture(gPosition, offset.xy).xyz;
        float sampleDepth = (view * vec4(fetchedWorld, 1.0)).z;

        float rangeCheck = smoothstep(0.0, 1.0, radius / (abs(fragPos.z - sampleDepth) + 0.0001));

        occlusion += (sampleDepth <= samplePos.z - bias ? 0.0 : 1.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / float(KERNEL_SIZE));
    FragColor = occlusion;
}