in vec2 TexCoords;

uniform sampler2D screenTexture;

#var vec2 inverseScreenSize
#var float aaThreshold

out vec4 FragColor;

float luma(vec3 c)
{
    return dot(c, vec3(0.299, 0.587, 0.114));
}

void main() {
    float L  = luma(texture(screenTexture, TexCoords).rgb);
    float Lr = luma(texture(screenTexture, TexCoords + vec2(inverseScreenSize.x, 0)).rgb);
    float Lt = luma(texture(screenTexture, TexCoords + vec2(0, inverseScreenSize.y)).rgb);

    float edgeH = smoothstep(aaThreshold, aaThreshold * 2.0, abs(L - Lr));
    float edgeV = smoothstep(aaThreshold, aaThreshold * 2.0, abs(L - Lt));

    FragColor = vec4(
        edgeH,
        edgeV,
        0.0,
        1.0
    );
}
