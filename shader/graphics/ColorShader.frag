uniform vec3 objectColor;

out vec4 FragColor;

#include "Color.glsl"

void main()
{
    FragColor = vec4(toLinear(objectColor), 1.0f);
}