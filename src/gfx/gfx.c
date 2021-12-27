#include "gfx/gfx.h"
#include "math/types.h"
#include "core/memory.h"

u32 gfx_get_bits_per_pixel(enum gfx_pixel_format pf)
{
	u32 bpp = 0;
	switch (pf) {
	case GFX_FORMAT_BGRA:
		bpp = 32;
		break;
	case GFX_FORMAT_RGBA:
		bpp = 32;
		break;
	case GFX_FORMAT_ARGB:
		bpp = 32;
		break;
	case GFX_FORMAT_RGB24:
		bpp = 24;
		break;
	case GFX_FORMAT_NV12:
		bpp = 16;
		break;
	case GFX_FORMAT_DEPTH_U16:
		bpp = 16;
		break;
	case GFX_FORMAT_DEPTH_U24:
		bpp = 24;
		break;
	case GFX_FORMAT_DEPTH_F32:
		bpp = 32;
		break;
	case GFX_FORMAT_UNKNOWN:
	default:
		break;
	}
	return bpp;
}

enum gfx_vertex_type gfx_vertex_type_from_string(const char* s)
{
	if (!strcmp(s, "posuv"))
		return GFX_VERTEX_POS_UV;
	else if (!strcmp(s, "poscol"))
		return GFX_VERTEX_POS_COLOR;
	return GFX_VERTEX_UNKNOWN;
}

u32 gfx_get_vertex_stride(enum gfx_vertex_type type)
{
	u32 stride = 0;
	switch (type) {
	case GFX_VERTEX_POS_UV:
		stride = sizeof(struct vec3f) + sizeof(struct vec2f);
		break;
	case GFX_VERTEX_POS_NORM_UV:
		stride = (sizeof(struct vec3f) * 2) + sizeof(struct vec2f);
		break;
	case GFX_VERTEX_POS_COLOR:
		stride = sizeof(struct vec3f) + sizeof(struct vec4f);
		break;
	case GFX_VERTEX_POS_NORM_COLOR:
		stride = (sizeof(struct vec3f) * 2) + sizeof(struct vec4f);
		break;
	default:
	case GFX_VERTEX_UNKNOWN:
		stride = 0;
		break;
	}
	return stride;
}

void gfx_init_sprite(gfx_system_t* gfx, gfx_buffer_t* vertex_buffer)
{
	size_t sz = sizeof(struct gfx_vertex_data);
	struct gfx_vertex_data* vd = (struct gfx_vertex_data*)bm_malloc(sz);
	memset(vd, 0, sz);
	vd->num_vertices = 4;
	size_t sz_positions = sizeof(vec3f_t) * vd->num_vertices;
	vd->positions = (vec3f_t*)bm_malloc(sz_positions);
	vd->tex_verts = (struct texture_vertex*)bm_malloc(
		sizeof(struct texture_vertex));
	size_t sz_tex_verts = sizeof(vec2f_t) * 4;
	vd->tex_verts->data = bm_malloc(sz_tex_verts);
	vd->tex_verts->size = sizeof(vec2f_t);
	gfx_create_buffer(gfx, vd, sz_positions + sz_tex_verts,
			  GFX_BUFFER_VERTEX, GFX_BUFFER_USAGE_DYNAMIC,
			  &vertex_buffer);
}

void gfx_draw_sprite(gfx_system_t* gfx, struct gfx_texture* texture, u32 width,
		     u32 height, u32 flags)
{
}
