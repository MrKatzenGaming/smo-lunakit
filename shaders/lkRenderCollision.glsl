#version 450 core

#include "lkCommon.glsl"
#include "lkModelBase.glsl"

DECLARE_VARYING(vec3, vWorldPos);
DECLARE_VARYING(vec3, vWorldNormal);
DECLARE_VARYING(vec3, vEdgeDistance);

#if VERTEX_SHADER

layout(std140, binding=0) uniform Mdl {
    mat3x4 cTransform;
};

layout (location=0) in vec3 aPosition;
layout (location=1) in vec3 aNormal;
layout (location=2) in float aEdgeDistance;

void main() {
    vWorldPos = vec4(aPosition, 1.0) * cTransform;
    gl_Position = vec4(vWorldPos, 1.0) * cViewProj;

    vWorldNormal = aNormal * mat3(cTransform);

    int vtxID = gl_VertexID % 3;
    vEdgeDistance = vec3(
        vtxID == 0 ? aEdgeDistance : 0.0, 
        vtxID == 1 ? aEdgeDistance : 0.0,
        vtxID == 2 ? aEdgeDistance : 0.0);
}

#elif FRAGMENT_SHADER

const float DiffuseMinLight = 0.3;
const float DiffuseMaxLight = 0.8;

const float OutlineThickness = 5.0;
const float OutlineSmoothing = 1.0;
const float OutlineDarkenFactor = 0.5;

layout (location=0) out vec4 fColor;

float edgeDistance(float bary, vec3 dPdx, vec3 dPdy, float dBdx, float dBdy) {
    float g00 = dot(dPdx, dPdx);
    float g01 = dot(dPdx, dPdy);
    float g10 = dot(dPdy, dPdx);
    float g11 = dot(dPdy, dPdy);
    float det = g00 * g11 - g01 * g10;

    float gradSq = (g00 * dBdy * dBdy
                  - g01 * dBdx * dBdy 
                  - g10 * dBdy * dBdx
                  + g11 * dBdx * dBdx) / det;
    return bary / sqrt(gradSq);
}

void main() {
    // Use "Blender" shading, simulating a light from the camera
    vec3 viewDir = normalize(vec4(vWorldPos, 1.0) * cView);
    vec3 viewNormal = normalize(vWorldNormal * mat3(cView));
    float diffuse = clamp(dot(viewNormal, -viewDir), DiffuseMinLight, DiffuseMaxLight);

    // Use distance from edge to apply an outline
    vec3 distDelta = fwidth(vEdgeDistance);
    vec3 outlines = smoothstep(OutlineThickness - distDelta * OutlineSmoothing, OutlineThickness + distDelta * OutlineSmoothing, vEdgeDistance);
    float outline = min(outlines.x, min(outlines.y, outlines.z));

    vec3 color = vec3(1.0, 1.0, 1.0);

    fColor = vec4(mix(color * OutlineDarkenFactor, color, outline) * diffuse, 1.0);
}

#endif // *_SHADER