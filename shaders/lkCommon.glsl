#ifndef COMMON_GLSL
#define COMMON_GLSL

// These are defined through the shader compiler
#define VERTEX_SHADER 0
#define FRAGMENT_SHADER 0
#define GEOMETRY_SHADER 0
#define COMPUTE_SHADER 0

#if VERTEX_SHADER

#define	DECLARE_VARYING(type, name)	out type name

#elif FRAGMENT_SHADER

#define	DECLARE_VARYING(type, name)	 in type name

#endif // *_SHADER

#endif // COMMON_GLSL
