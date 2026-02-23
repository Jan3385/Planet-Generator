in vec2 TexCoords;

uniform sampler2D uEdgeTex;

#var vec2 inverseScreenSize

#define MAX_SEARCH_STEPS 12

out vec4 FragColor;

void main()
{
    vec2 edge = texture(uEdgeTex, TexCoords).rg;

    float weightH = 0.0;
    float weightV = 0.0;

    // Horizontal search
    if (edge.r > 0.001)
    {
        float leftLength = 0.0;
        float rightLength = 0.0;

        for (int i = 1; i <= MAX_SEARCH_STEPS; ++i)
        {
            float sampleEdge = texture(uEdgeTex,
                TexCoords - vec2(i * inverseScreenSize.x, 0)).r;

            if (sampleEdge > 0.001)
                leftLength += sampleEdge;
            else
                break;
        }

        for (int i = 1; i <= MAX_SEARCH_STEPS; ++i)
        {
            float sampleEdge = texture(uEdgeTex,
                TexCoords + vec2(i * inverseScreenSize.x, 0)).r;

            if (sampleEdge > 0.001)
                rightLength += sampleEdge;
            else
                break;
        }

        weightH = leftLength + rightLength + 1.0;
    }

    // Vertical search
    if (edge.g > 0.001)
    {
        float upLength = 0.0;
        float downLength = 0.0;

        for (int i = 1; i <= MAX_SEARCH_STEPS; ++i)
        {
            float sampleEdge = texture(uEdgeTex,
                TexCoords - vec2(0, i * inverseScreenSize.y)).g;

            if (sampleEdge > 0.001)
                upLength += sampleEdge;
            else
                break;
        }

        for (int i = 1; i <= MAX_SEARCH_STEPS; ++i)
        {
            float sampleEdge = texture(uEdgeTex,
                TexCoords + vec2(0, i * inverseScreenSize.y)).g;

            if (sampleEdge > 0.001)
                downLength += sampleEdge;
            else
                break;
        }
        
        weightV = upLength + downLength + 1.0;
    }

    FragColor = vec4(weightH, weightV, 0.0, 1.0);
}
