in vec3 FragPos;

uniform vec3 pointLightPos;
uniform float farPlane;

void main()
{
    float lightDistance = length(FragPos.xyz - pointLightPos);

    lightDistance = lightDistance / farPlane;

    gl_FragDepth = lightDistance;
}