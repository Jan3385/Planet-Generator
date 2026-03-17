in vec2 TexCoords;

uniform sampler2D ssaoInput;

out float FragColor;

void main(){
    vec2 texelSize = 1.0 / textureSize(ssaoInput, 0);
    float result = 0.0;

    const int sampleRadius = 2;

    for(int x = -sampleRadius; x <= sampleRadius; ++x){
        for(int y = -sampleRadius; y <= sampleRadius; ++y){
            vec2 offset = vec2(x, y) * texelSize;
            result += texture(ssaoInput, TexCoords + offset).r;
        }
    }
    
    const int sampleCount = (sampleRadius * 2 + 1) * (sampleRadius * 2 + 1);
    FragColor = result / sampleCount;
}