#include "gfx/gfx.h"
#include "math/types.h"
#include "core/memory.h"

gfx_system_t* gfx = NULL;

size_t gfx_get_shader_var_size(enum gfx_shader_var_type type)
{
	switch (type) {
	case GFX_SHADER_PARAM_BOOL:
		return sizeof(bool);
	case GFX_SHADER_PARAM_S32:
		return sizeof(s32);
	case GFX_SHADER_PARAM_U32:
		return sizeof(u32);
	case GFX_SHADER_PARAM_F32:
		return sizeof(f32);
	case GFX_SHADER_PARAM_F64:
		return sizeof(f64);
	case GFX_SHADER_PARAM_VEC2:
		return sizeof(struct vec2f);
	case GFX_SHADER_PARAM_VEC3:
		return sizeof(struct vec3f);
	case GFX_SHADER_PARAM_VEC4:
		return sizeof(struct vec4f);
	case GFX_SHADER_PARAM_MAT4:
		return sizeof(struct mat4f);
	case GFX_SHADER_PARAM_TEX:
		return sizeof(void*);
	}
	return 0;
}

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

result gfx_init(const struct gfx_config* cfg,
					s32 flags)
{
	if (cfg->module == GFX_MODULE_DX11) {
		gfx = (gfx_system_t*)mem_alloc(sizeof(gfx_system_t));
		memset(gfx, 0, sizeof(*gfx));
		return gfx_init_dx11(cfg, flags);
	}
	return RESULT_NOT_IMPL;
}

void gfx_shutdown(void)
{
	if (gfx) {
		gfx_destroy_render_target();
		gfx_destroy_zstencil();
		gfx_destroy_device_dependent_resources();
		gfx_shutdown_dx11();
	}
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

void gfx_init_sprite(gfx_buffer_t* vertex_buffer)
{
	size_t sz = sizeof(struct gfx_vertex_data);
	struct gfx_vertex_data* vd = (struct gfx_vertex_data*)mem_alloc(sz);
	memset(vd, 0, sz);
	vd->num_vertices = 4;
	size_t sz_positions = sizeof(vec3f_t) * vd->num_vertices;
	vd->positions = (vec3f_t*)mem_alloc(sz_positions);
	vd->tex_verts = (struct texture_vertex*)mem_alloc(
		sizeof(struct texture_vertex));
	size_t sz_tex_verts = sizeof(vec2f_t) * 4;
	vd->tex_verts->data = mem_alloc(sz_tex_verts);
	vd->tex_verts->size = sizeof(vec2f_t);
	gfx_buffer_create(gfx, vd, sz_positions + sz_tex_verts,
			  GFX_BUFFER_VERTEX, GFX_BUFFER_USAGE_DYNAMIC,
			  &vertex_buffer);
}

void gfx_draw_sprite(struct gfx_texture* texture, u32 width,
		     u32 height, u32 flags)
{
}

const char* gfx_shader_type_to_string(enum gfx_shader_type type)
{
	switch (type) {
		case GFX_SHADER_VERTEX: return "vertex";
		case GFX_SHADER_PIXEL: return "pixel";
		case GFX_SHADER_GEOMETRY: return "geometry";
		case GFX_SHADER_COMPUTE: return "compute";
	}
	return NULL;
}
