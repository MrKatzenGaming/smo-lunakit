#ifndef MODEL_BASE_GLSL
#define MODEL_BASE_GLSL

#include "lkCommon.glsl"

// Source:
// * https://github.com/GRAnimated/NintendoShaders/tree/main/al/RedStar/Shaders
// * https://github.com/KillzXGaming/ShaderLibrary/tree/main/ShaderLibrary.CompileTool/Shaders/SMO

layout(std140, binding=1) uniform MdlEnvView {
	mat3x4	cView;
	mat3x4	cInvView;
	mat4x4	cViewProj;
	mat3x4	cInvProjView;
	mat4x4	cInvProj;
	mat3x4	uInvProjViewNoTrans;

	float	cInvExposure;
	float	uIrradianceScale;

	vec4	cDirLightViewDirFetchPos;

	float	cNear;
	float	cFar;
	float	cRange;		// == (far - near)
	float	cInvRange;	// == 1 / (far - near)
	vec2	cTanFovyHalf;	// tan( fovy / 2 )
	vec2	cScrProjOffset;
	vec4	cScrSize;	// xy : screen size,  zw : inv screen size

	vec3	cCameraPos;
	vec4	uAlphaMaskProjMtx[4];

	vec4	cParallaxCubeInvWorldMtx[3];
	vec3	cParallaxCubeCenter;
	vec3	cParallaxCubeHalfSize;

	int		cBayerMtxSize;
	int		cBayerMtx[256];

	mat3x4	cPrevView;
	mat4x4	cPrevViewProj;

	float	cGlobalLodBias;
	vec2	cDitherScale;

	float 	cInvNearClipWidth;
};

#if VERTEX_SHADER
#elif FRAGMENT_SHADER

layout(std140, binding=7) uniform HDRTranslate {
	float	uHDRPower;
	float	uDynamicRange;
};

layout (location=0) out vec4 fLightBuf;
layout (location=1) out vec4 fWorldNrm;
layout (location=2) out vec4 fDepthView;
layout (location=3) out vec4 fBaseColor;

vec4 DecodeCubemap(samplerCube cube, vec3 n, float lod) {
    vec4 tex = textureLod(cube, n, lod);

    float scale = pow(tex.a, uHDRPower) * uDynamicRange;
    return vec4(tex.rgb * scale, scale);
}

struct Light {
    vec3 I; //eye
    vec3 N; //normal
    vec3 V; //view
    vec3 H; //half
    vec3 L; //light
    vec3 R; //reflect
    float NV; //dot normal view
};
Light SetupLight(vec3 N, vec3 viewPos) {
    Light light;

    vec3 dir = normalize(viewPos);
    vec3 viewNormal = normalize(N * mat3(cView));
    vec3 cubemapCoords = reflect(dir, viewNormal.xyz) * mat3(cInvView);

    light.N = N;
    light.I = vec3(0.0, 0.0, 1.0) * mat3(cView);
    light.V = normalize(light.I); // view
    light.L = normalize(cDirLightViewDirFetchPos.xyz); // Light
    light.H = normalize(light.V + light.L); // half angle
    light.R = vec3(cubemapCoords.x, cubemapCoords.y, -cubemapCoords.z); // reflection
    light.NV = saturate(dot(light.N, light.V));

    return light;
}

vec3 CalculateBrdf(vec3 view_normal, vec3 dir, float roughness, vec3 f0)
{
    float r = (1.0 - roughness);
    float a = r * r;
    float a2 = a * a;

    float nv = dot(view_normal, -dir);

    float s = clamp(min(a2 * fma(a2, 1.895, -0.1688),
        fma(nv, fma(nv, fma(nv, -5.069, 8.404), -4.853), 0.9903)) + 0.0, 0.0, 1.0);

    float b = clamp(fma(nv, fma(nv, 0.1939, -0.5228), a2 *
        (fma(a2, fma(a2, 2.661, -3.603), nv * 1.404) + 1.699)) + 0.6045, 0.0, 1.0) - s;

    return f0.rgb * b + s * saturate(f0.g * 50.0);
}

#endif // *_SHADER

#endif // MODEL_BASE_GLSL