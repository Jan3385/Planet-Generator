layout (location = 0) in vec3 aPos;

uniform mat4 transform;
#var mat4 dirLightSpaceMatrix

void main()
{
    gl_Position = dirLightSpaceMatrix * transform * vec4(aPos.xyz, 1.0);
}  