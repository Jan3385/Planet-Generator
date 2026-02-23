layout (location = 0) in vec3 aPos;

uniform mat4 transform;
uniform mat4 prevTransform;

#var mat4 projection
#var mat4 view
#var mat4 previousProjection
#var mat4 previousView

out vec4 vCurrClip;
out vec4 vPrevClip;

void main()
{
    vec4 worldPos = transform * vec4(aPos.xyz, 1.0);

    gl_Position = projection * view * worldPos;

    vCurrClip = projection * view * worldPos;
    vPrevClip = previousProjection * previousView * prevTransform * vec4(aPos.xyz, 1.0);
}