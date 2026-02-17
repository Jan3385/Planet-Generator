in vec4 vCurrClip;
in vec4 vPrevClip;

out vec4 FragColor;

void main()
{
    vec2 currNDC = vCurrClip.xy / vCurrClip.w;
    vec2 prevNDC = vPrevClip.xy / vPrevClip.w;
    FragColor = vec4((currNDC - prevNDC) * 0.5, 0.0, 1.0);
} 