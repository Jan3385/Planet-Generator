#define BACKFACE_BIAS 0.0001

layout (depth_greater) out float gl_FragDepth;

void main()
{             
    gl_FragDepth = gl_FragCoord.z;
    gl_FragDepth += gl_FrontFacing ? BACKFACE_BIAS : 0;
}  