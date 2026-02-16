in vec2 TexCoords;

uniform sampler2D screenTexture;

#var float gamma
#var float exposure
#var vec2 inverseScreenSize

#get FXAA_ANTIALIASING

out vec4 FragColor;

void main()
{ 
    vec3 HDRColor = texture(screenTexture, TexCoords).rgb;

    // FXAA anti-aliasing
#if FXAA_ANTIALIASING
    vec3 rgbN = texture(screenTexture, TexCoords + vec2(0.0, -inverseScreenSize.y)).rgb;
    vec3 rgbS = texture(screenTexture, TexCoords + vec2(0.0, inverseScreenSize.y)).rgb;
    vec3 rgbE = texture(screenTexture, TexCoords + vec2(inverseScreenSize.x, 0.0)).rgb;
    vec3 rgbW = texture(screenTexture, TexCoords + vec2(-inverseScreenSize.x, 0.0)).rgb;

    const vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaM = dot(HDRColor, luma);
    float lumaN = dot(rgbN, luma);
    float lumaS = dot(rgbS, luma);
    float lumaE = dot(rgbE, luma);
    float lumaW = dot(rgbW, luma);

    float lumaMin = min(lumaM, min(min(lumaN, lumaS), min(lumaE, lumaW)));
    float lumaMax = max(lumaM, max(max(lumaN, lumaS), max(lumaE, lumaW)));
    float lumaRange = lumaMax - lumaMin;

    if (lumaRange > 0.1){
        vec3 filtered = (HDRColor + rgbN + rgbS + rgbE + rgbW) / 5.0;
        HDRColor = filtered;
    }
#endif

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-HDRColor * exposure);

    // gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped, 1.0);
}
