#ifndef COMMON_GLSL
#define COMMON_GLSL

// These are defined through the shader compiler
#define VERTEX_SHADER 0
#define FRAGMENT_SHADER 0
#define GEOMETRY_SHADER 0
#define COMPUTE_SHADER 0

#if VERTEX_SHADER

#define	DECLARE_VARYING(type, name)                      out type name
#define	DECLARE_NOPERS_VARYING(type, name) noperspective out type name

#elif FRAGMENT_SHADER

#define	DECLARE_VARYING(type, name)                      in type name
#define	DECLARE_NOPERS_VARYING(type, name) noperspective in type name

#endif // *_SHADER

float saturate(float v) {
    return clamp(v, 0.0, 1.0);
}
vec2 saturate(vec2 v) {
    return clamp(v, 0.0, 1.0);
}
vec3 saturate(vec3 v) {
    return clamp(v, 0.0, 1.0);
}

#endif // COMMON_GLSL
