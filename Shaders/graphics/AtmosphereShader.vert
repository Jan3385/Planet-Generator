layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 transform;
uniform mat3 normalMatrix;

#var mat4 projection
#var mat4 view

out vec3 Normal;
out vec3 FragPos;

void main()
{
    gl_Position = projection * view * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    Normal = normalize(normalMatrix * aNormal);
    FragPos = vec3(transform * vec4(aPos, 1.0));
}