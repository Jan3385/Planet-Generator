in vec2 TexCoords;

uniform sampler2D uCurrent;
uniform sampler2D uHistory;
uniform sampler2D uVelocity;

#var vec2 inverseScreenSize
#var float aaThreshold

out vec4 FragColor;

void main()
{ 
    vec2 velocity = texture(uVelocity, TexCoords).xy;
    vec2 historyUV = clamp(TexCoords - velocity, vec2(0.0), vec2(1.0));

    vec3 currentColor = texture(uCurrent, TexCoords).rgb;
    vec3 historyColor = texture(uHistory, historyUV).rgb;

    // neighborhood clamp
    vec3 minC = vec3(999.0);
    vec3 maxC = vec3(-999.0);
    for (int x = -1; x <= 1; x++){
        for (int y = -1; y <= 1; y++){
            vec2 offset = vec2(x, y) * inverseScreenSize;
            vec3 neighborColor = texture(uCurrent, TexCoords + offset).rgb;
            minC = min(minC, neighborColor);
            maxC = max(maxC, neighborColor);
        }
    }
    historyColor = clamp(historyColor, minC, maxC);

    // blend
    vec3 result = mix(currentColor, historyColor, 1.0 - aaThreshold);

    if(aaThreshold > 0.001)
        FragColor = vec4(result, 1.0);
    else
        FragColor = vec4(texture(uVelocity, TexCoords).xy, 0.0, 1.0); // debug velocity to output
}