#version 450 core

#include "lkCommon.glsl"

#if VERTEX_SHADER

layout(std140, binding=0) uniform Context {
    mat4 uProjViewWorld;
};

layout (location=0) in vec4 aPosition;

void main() {
    gl_Position = vec4(aPosition.xyz, 1.0) * uProjViewWorld;
}

#elif FRAGMENT_SHADER

out vec4 fColor;

void main() {
    fColor = vec4(1.0, 0.0, 0.0, 1.0);
}

#endif // *_SHADER