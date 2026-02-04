in vec2 TexCoords;

uniform sampler2D screenTexture;

#var float gamma
#var float exposure

out vec4 FragColor;

void main()
{ 
    vec3 HDRColor = texture(screenTexture, TexCoords).rgb;

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-HDRColor * exposure);

    // gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped, 1.0);
}
