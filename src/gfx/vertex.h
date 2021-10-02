#ifndef H_GFX_VERTEX
#define H_GFX_VERTEX

#include "core/types.h"

enum gfx_vertex_type {
    GFX_VERTEX_POS_UV = 0,
    GFX_VERTEX_FIRST = GFX_VERTEX_POS_UV,
    GFX_VERTEX_POS_NORM_UV = 1,
    GFX_VERTEX_POS_COLOR = 2,
    GFX_VERTEX_POS_NORM_COLOR = 3,
    GFX_VERTEX_UNKNOWN = 4,
    GFX_VERTEX_LAST = GFX_VERTEX_UNKNOWN
};

struct gs_tvertarray {
	size_t width;
	void *array;
};

struct gs_vb_data {
	size_t num;
	struct vec3 *points;
	struct vec3 *normals;
	struct vec3 *tangents;
	uint32_t *colors;

	size_t num_tex;
	struct gs_tvertarray *tvarray;
};


struct gfx_vertex_data {
	size_t num;
	struct vec3* points;
	struct vec3* normals;
	struct vec3* tangents;
	u32 *colors;

    size_t num_tex_verts;
    void* tex_verts;
	// size_t num_tex;
	// struct gs_tvertarray *tvarray;
};

// struct VertexPosUV {
//     math::Vec3 position;
//     math::Vec2 tex_coord;
// };

// struct VertexPosNormalUV {
//     math::Vec3 position;
//     math::Vec3 normal;
//     math::Vec3 tex_coord;
// };

// struct VertexPosColor {
//     math::Vec3 position;
//     math::Vec4 color;
// };

// struct VertexPosNormalColor {
//     math::Vec3 position;
//     math::Vec3 normal;
//     math::Vec4 color;
// };

#endif
