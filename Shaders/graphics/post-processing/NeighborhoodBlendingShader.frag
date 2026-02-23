in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D uBlendWeightTex;

#var vec2 inverseScreenSize

out vec4 FragColor;

void main() {
    vec3 color = texture(screenTexture, TexCoords).rgb;
    vec2 blendWeights = texture(uBlendWeightTex, TexCoords).rg;

    // Horizontal blending
    if (blendWeights.r > 0.0)
    {
        float offset = blendWeights.r;
        vec3 left  = texture(screenTexture, TexCoords - vec2(offset * inverseScreenSize.x, 0)).rgb;
        vec3 right = texture(screenTexture, TexCoords + vec2(offset * inverseScreenSize.x, 0)).rgb;
        color = (left + right) * 0.5;
    }
    // Vertical blending
    if (blendWeights.g > 0.0)
    {
        float offset = blendWeights.g;
        vec3 up   = texture(screenTexture, TexCoords - vec2(0, offset * inverseScreenSize.y)).rgb;
        vec3 down = texture(screenTexture, TexCoords + vec2(0, offset * inverseScreenSize.y)).rgb;
        color = (up + down) * 0.5;
    }

    FragColor = vec4(color, 1.0);
}
