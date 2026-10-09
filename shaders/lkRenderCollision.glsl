#version 450 core

#include "lkCommon.glsl"

#define RENDER_TYPE (0)
#define RENDER_TYPE_NORMAL (0)
#define RENDER_TYPE_DEPTH (1)

DECLARE_VARYING(vec3, vNormal);

#if VERTEX_SHADER

layout(std140, binding=0) uniform Context {
    mat4 uProjViewWorld;
};

layout (location=0) in vec3 aPosition;
layout (location=0) in vec3 aNormal;

void main() {
    gl_Position = vec4(aPosition, 1.0) * uProjViewWorld;
#if RENDER_TYPE == RENDER_TYPE_NORMAL
    vNormal = aNormal;
#endif
}

#elif FRAGMENT_SHADER

layout (location=0) out vec4 fLight;
layout (location=1) out vec4 fNormal;
layout (location=3) out vec4 fAlbedo;

void main() {
#if RENDER_TYPE == RENDER_TYPE_NORMAL
    fLight = vec4(1.0, 0.0, 0.0, 1.0);
    fAlbedo = vec4(1.0, 0.0, 0.0, 1.0);

    fNormal.rg = vNormal.rg * 0.5 + 0.5;
#elif RENDER_TYPE == RENDER_TYPE_DEPTH
    fLight = vec4(0.0, 0.0, 0.0, 1.0);
#endif
}

#endif // *_SHADER