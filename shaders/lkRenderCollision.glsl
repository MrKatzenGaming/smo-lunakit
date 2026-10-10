#version 450 core

#include "lkCommon.glsl"
#include "lkModelBase.glsl"

#define RENDER_TYPE (0)
#define RENDER_TYPE_NORMAL (0)
#define RENDER_TYPE_DEPTH (1)

DECLARE_VARYING(vec4, vNormalDepth); // .xyz normal, .w depth
DECLARE_VARYING(vec3, vViewPos);

#if VERTEX_SHADER

layout(std140, binding=0) uniform Mdl {
    mat3x4 cTransform;
};

layout (location=0) in vec3 aPosition;
layout (location=1) in vec3 aNormal;

void main() {
    vec3 modelPos = vec4(aPosition, 1.0) * cTransform;
    gl_Position   = vec4(modelPos, 1.0) * cViewProj;

#if RENDER_TYPE == RENDER_TYPE_NORMAL
    vViewPos      = vec4(modelPos, 1.0) * cView;

    vNormalDepth.xyz = aNormal;
    vNormalDepth.w = (gl_Position.w - cNear) * cInvRange;
#endif
}

#elif FRAGMENT_SHADER

void main() {
#if RENDER_TYPE == RENDER_TYPE_NORMAL
    fBaseColor = vec4(1.0, 0.0, 0.0, 1.0);

    float roughness = 0.0;
    float metalness = 0.0;

    vec3 viewNormal = normalize(vNormalDepth.xyz * mat3(cView));
    vec3 viewDir = normalize(vViewPos);

    vec3 f0 = mix(vec3(0.04), fBaseColor.rgb, metalness);
    vec3 brdf = CalculateBrdf(viewNormal, viewDir, roughness, f0);

    vec4 irradiance = vec4(1.0) * uIrradianceScale;
    vec3 diffuse  = saturate(fBaseColor.rgb);
         diffuse *= vec3(1.0) - brdf;
         diffuse *= irradiance.rgb;

    fLightBuf.rgb = diffuse;
    fLightBuf.a = fBaseColor.a;

    fWorldNrm.xy = vNormalDepth.xy * 0.5 + 0.5;
    fDepthView.x = vNormalDepth.w;
#elif RENDER_TYPE == RENDER_TYPE_DEPTH
    fLightBuf = vec4(0.0, 0.0, 0.0, 1.0);
#endif
}

#endif // *_SHADER