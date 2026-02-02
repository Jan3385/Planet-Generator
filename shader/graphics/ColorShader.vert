layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

#var mat4 projection
#var mat4 view
uniform mat4 transform;

void main()
{
    gl_Position = projection * view * transform * vec4(aPos.xyz, 1.0);
}