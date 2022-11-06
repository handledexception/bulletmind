#include "gfx/gfx.h"
#include "math/types.h"
#include "core/memory.h"
#include "media/image.h"

gfx_system_t* gfx = NULL;
bool gfx_hardware_ready = false;
bool gfx_system_ready = false;

gfx_mesh_t* gfx_mesh_new(enum gfx_vertex_type type, u32 num_verts)
{
	struct gfx_mesh* data = BM_ALLOC(sizeof(struct gfx_mesh));
	data->type = type;
	data->num_vertices = num_verts;
	data->positions = NULL;
	data->colors = NULL;
	data->normals = NULL;
	data->tangents = NULL;
	data->tex_verts = NULL;

	if (VERTEX_HAS_POS(type)) {
		size_t sz_pos = sizeof(struct vec3f) * num_verts;
		data->positions = BM_ALLOC(sz_pos);
		memset(data->positions, 0, sz_pos);
	}
	if (VERTEX_HAS_NORMAL(type)) {
		size_t sz_norm = sizeof(struct vec3f) * num_verts;
		data->normals = BM_ALLOC(sz_norm);
		memset(data->normals, 0, sz_norm);
	}
	if (VERTEX_HAS_COLOR(type)) {
		size_t sz_col = sizeof(struct vec4f) * num_verts;
		data->colors = BM_ALLOC(sz_col);
		memset(data->colors, 0, sz_col);
	}
	if (VERTEX_HAS_UV(type)) {
		data->tex_verts =
			BM_ALLOC(sizeof(struct texture_vertex) * num_verts);
		size_t uv_size = sizeof(struct vec2f);
		for (size_t i = 0; i < num_verts; i++) {
			data->tex_verts[i].data = BM_ALLOC(uv_size);
			memset(data->tex_verts[i].data, 0, sizeof(uv_size));
			data->tex_verts[i].size = uv_size;
		}
	}

	return data;
}
void gfx_mesh_free(gfx_mesh_t* data)
{
	if (data != NULL) {
		if (data->positions != NULL) {
			BM_FREE(data->positions);
			data->positions = NULL;
		}
		if (data->normals != NULL) {
			BM_FREE(data->normals);
			data->normals = NULL;
		}
		if (data->tangents != NULL) {
			BM_FREE(data->tangents);
			data->tangents = NULL;
		}
		if (data->colors != NULL) {
			BM_FREE(data->colors);
			data->colors = NULL;
		}
		if (data->tex_verts != NULL) {
			for (size_t i = 0; i < data->num_vertices; i++) {
				if (data->tex_verts[i].data != NULL) {
					BM_FREE(data->tex_verts[i].data);
					data->tex_verts[i].data = NULL;
					data->tex_verts[i].size = 0;
				}
			}
			BM_FREE(data->tex_verts);
			data->tex_verts = NULL;
		}
		BM_FREE(data);
		data = NULL;
	}
}

size_t gfx_mesh_get_size(const gfx_mesh_t* mesh)
{
	size_t size = 0;
	if (mesh) {
		size += sizeof(struct gfx_mesh);
		if (VERTEX_HAS_POS(mesh->type)) {
			size += sizeof(vec3f_t) * mesh->num_vertices;
		}
		if (VERTEX_HAS_COLOR(mesh->type)) {
			size += sizeof(vec4f_t) * mesh->num_vertices;
		}
		if (VERTEX_HAS_NORMAL(mesh->type)) {
			size += sizeof(vec3f_t) * mesh->num_vertices;
		}
		if (VERTEX_HAS_TANGENT(mesh->type)) {
			size += sizeof(vec3f_t) * mesh->num_vertices;
		}
		if (VERTEX_HAS_UV(mesh->type)) {
			for (size_t i = 0; i < mesh->num_vertices; i++) {
				size += sizeof(vec2f_t) +
					sizeof(struct texture_vertex);
			}
		}
	}

	return size;
}

size_t gfx_shader_var_size(enum gfx_shader_var_type type)
{
	switch (type) {
	case GFX_SHADER_VAR_BOOL:
		return sizeof(bool);
	case GFX_SHADER_VAR_S32:
		return sizeof(s32);
	case GFX_SHADER_VAR_U32:
		return sizeof(u32);
	case GFX_SHADER_VAR_F32:
		return sizeof(f32);
	case GFX_SHADER_VAR_F64:
		return sizeof(f64);
	case GFX_SHADER_VAR_VEC2:
		return sizeof(struct vec2f);
	case GFX_SHADER_VAR_VEC3:
		return sizeof(struct vec3f);
	case GFX_SHADER_VAR_VEC4:
		return sizeof(struct vec4f);
	case GFX_SHADER_VAR_MAT4:
		return sizeof(struct mat4f);
	case GFX_SHADER_VAR_TEX:
		return sizeof(void*);
	}
	return 0;
}

result gfx_init(const struct gfx_config* cfg, s32 flags)
{
	if (cfg->module == GFX_MODULE_DX11) {
		gfx = (gfx_system_t*)BM_ALLOC(sizeof(gfx_system_t));
		memset(gfx, 0, sizeof(*gfx));
		result res = gfx_init_dx11(cfg, flags);
		if (res == RESULT_OK)
			res = gfx_init_renderer(cfg, flags);
		if (res != RESULT_OK) {
			logger(LOG_ERROR,
			       "\033[7mgfx\033[m Error initializing Direct3D11!");
			gfx_shutdown_dx11();
		}
		return res;
	}
	return RESULT_NOT_IMPL;
}

void gfx_shutdown(void)
{
	if (gfx) {
		gfx_render_target_destroy();
		gfx_destroy_depth();
		gfx_destroy_device();
		if (gfx->type == GFX_MODULE_DX11)
			gfx_shutdown_dx11();
		BM_FREE(gfx);
		gfx = NULL;
	}
}

bool gfx_hardware_ok(void)
{
	return gfx_hardware_ready;
}

bool gfx_ok(void)
{
	// Should be OK if gfx initialization completed successfully
	return gfx_hardware_ready && gfx_system_ready;
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

gfx_shader_var_t* gfx_shader_var_new(const char* name,
				     enum gfx_shader_var_type type)
{
	gfx_shader_var_t* var = BM_ALLOC(sizeof(*var));
	gfx_shader_var_init(var);
	size_t len = strlen(name);
	if (len > 255) {
		strncpy(&var->name[0], name, 255);
		var->name[256] = '\0';
	} else {
		strncpy(&var->name[0], name, len);
	}

	return var;
}

void gfx_shader_var_init(gfx_shader_var_t* var)
{
	if (var != NULL) {
		var->data = NULL;
		memset(var->name, 0, 256);
		var->type = GFX_SHADER_VAR_UNKNOWN;
	}
}

void gfx_shader_var_free(gfx_shader_var_t* var)
{
	if (var != NULL) {
		if (var->data != NULL && var->own_data) {
			if (var->type == GFX_SHADER_VAR_TEX) {
				gfx_texture_destroy(var->data);
			} else {
				BM_FREE(var->data);
			}
			var->data = NULL;
		}
		// BM_FREE(var);
		// var = NULL;
	}
}

void gfx_shader_var_set(gfx_shader_var_t* var, const void* data)
{
	if (var != NULL && data != NULL) {
		size_t szd = gfx_shader_var_size(var->type);
		if (var->data != NULL && var->own_data) {
			BM_FREE(var->data);
			var->data = NULL;
		}
		if (var->data == NULL) {
			var->data = BM_ALLOC(szd);
		}
		memcpy(var->data, data, szd);
		var->own_data = true;
	}
}

void gfx_shader_var_set_from(gfx_shader_var_t* var, const void* data)
{
	if (var != NULL) {
		if (var->data != NULL && var->own_data) {
			BM_FREE(var->data);
			var->data = NULL;
		}
		var->data = data;
		var->own_data = false;
	}
}

bool gfx_shader_add_var(gfx_shader_t* shader, const gfx_shader_var_t var)
{
	// if (shader != NULL && var != NULL) {
	if (shader) {
		if (vec_find(shader->vars, &var, 0) == VEC_NOT_FOUND) {
			vec_push_back(shader->vars, &var);
			return true;
		}
	}
	return false;
}

bool gfx_shader_set_var_by_name(gfx_shader_t* shader, const char* name,
				const void* value, size_t size, bool own_data)
{
	for (size_t i = 0; i < shader->vars.num_elems; i++) {
		gfx_shader_var_t* var =
			(gfx_shader_var_t*)&shader->vars.elems[i];
		if (var != NULL && !strcmp(var->name, name)) {
			if (own_data)
				gfx_shader_var_set(var, value);
			else
				gfx_shader_var_set_from(var, value);
		}
		return true;
	}
	return false;
}

size_t gfx_shader_get_vars_size(gfx_shader_t* shader)
{
	if (shader == NULL)
		return 0;

	size_t buf_size = 0;
	for (size_t i = 0; i < shader->vars.num_elems; i++) {
		size_t var_size =
			gfx_shader_var_size(shader->vars.elems[i].type);
		buf_size += var_size;
	}

	if (buf_size > 0)
		buf_size = (buf_size + 15 & 0xfffffff0);

	return buf_size;
}

gfx_shader_var_t* gfx_shader_get_var_by_name(gfx_shader_t* shader,
					     const char* name)
{
	gfx_shader_var_t* found = NULL;
	for (size_t i = 0; i < shader->vars.num_elems; i++) {
		gfx_shader_var_t* var =
			(gfx_shader_var_t*)&shader->vars.elems[i];
		if (var != NULL && !strcmp(var->name, name)) {
			found = var;
			break;
		}
	}
	return found;
}

gfx_shader_t* gfx_shader_new(enum gfx_shader_type type)
{
	gfx_shader_t* shader = (gfx_shader_t*)BM_ALLOC(sizeof(gfx_shader_t));
	gfx_shader_init(shader);
	shader->type = type;
	switch (shader->type) {
	case GFX_SHADER_VERTEX:
		shader->impl = (gfx_vertex_shader_t*)gfx_vertex_shader_new();
		break;
	case GFX_SHADER_PIXEL:
		shader->impl = (gfx_pixel_shader_t*)gfx_pixel_shader_new();
		break;
	case GFX_SHADER_GEOMETRY:
	case GFX_SHADER_COMPUTE:
	default:
		break;
	}
	return shader;
}

gfx_shader_t* gfx_shader_adopt(gfx_shader_t* other)
{
	gfx_shader_t* shader = NULL;
	if (other != NULL) {
		shader = gfx_shader_new(GFX_SHADER_UNKNOWN);
		gfx_shader_init(shader);
		bool adopted = false;
		switch (other->type) {
		case GFX_SHADER_VERTEX:
			adopted = gfx_vertex_shader_acquire(other->impl);
			break;
		case GFX_SHADER_PIXEL:
			adopted = gfx_pixel_shader_acquire(other->impl);
			break;
		case GFX_SHADER_UNKNOWN:
		default:
			break;
		}
		if (adopted) {
			shader->impl = other->impl;
			shader->type = other->type;
		}
	}
	return shader;
}

gfx_shader_t* gfx_shader_copy(gfx_shader_t* other)
{
	if (other) {
	}
}

void gfx_shader_init(gfx_shader_t* shader)
{
	if (shader != NULL) {
		shader->type = GFX_SHADER_UNKNOWN;
		shader->impl = NULL;
		shader->cbuffer = NULL;
		vec_init(shader->vars);
	}
}

void gfx_init_sprite(gfx_buffer_t* vertex_buffer)
{
	size_t sz = sizeof(struct gfx_mesh);
	struct gfx_mesh* vd = (struct gfx_mesh*)BM_ALLOC(sz);
	memset(vd, 0, sz);
	vd->num_vertices = 4;
	size_t sz_positions = sizeof(vec3f_t) * vd->num_vertices;
	vd->positions = (vec3f_t*)BM_ALLOC(sz_positions);
	vd->tex_verts =
		(struct texture_vertex*)BM_ALLOC(sizeof(struct texture_vertex));
	size_t sz_tex_verts = sizeof(vec2f_t) * 4;
	vd->tex_verts->data = BM_ALLOC(sz_tex_verts);
	vd->tex_verts->size = sizeof(vec2f_t);
	gfx_buffer_new(vd, sz_positions + sz_tex_verts, GFX_BUFFER_VERTEX,
		       GFX_BUFFER_USAGE_DYNAMIC, &vertex_buffer);
}

// void gfx_destroy_sprite(gfx_buffer_t* vertex_buffer)
// {
// 	if (vertex_buffer) {
// 		gfx_buffer_free(vertex_buffer);
// 		BM_FREE(vertex_buffer)
// 	}
// }

void gfx_draw_sprite(struct gfx_texture* texture, u32 width, u32 height,
		     u32 flags)
{
}

const char* gfx_shader_type_to_string(enum gfx_shader_type type)
{
	switch (type) {
	case GFX_SHADER_VERTEX:
		return "vertex";
	case GFX_SHADER_PIXEL:
		return "pixel";
	case GFX_SHADER_GEOMETRY:
		return "geometry";
	case GFX_SHADER_COMPUTE:
		return "compute";
	}
	return NULL;
}

result gfx_texture_create(const u8* data, const struct gfx_texture_desc* desc,
			  gfx_texture_t** texture)
{
	result res = RESULT_OK;

	gfx_texture_t* tex = (gfx_texture_t*)BM_ALLOC(sizeof(*tex));
	gfx_texture_init(tex);

	switch (desc->type) {
	case GFX_TEXTURE_2D:
		res = gfx_texture2d_create(data, desc, &tex);
		tex->type = GFX_TEXTURE_2D;
		break;
	case GFX_TEXTURE_1D:
	case GFX_TEXTURE_3D:
	case GFX_TEXTURE_CUBE:
	default:
		res = RESULT_NOT_IMPL;
		break;
	}

	if (res == RESULT_OK) {
		*texture = tex;
	} else {
		BM_FREE(tex);
		tex = NULL;
	}

	return res;
}

//
// gfx texture
//
void gfx_texture_init(gfx_texture_t* texture)
{
	if (texture != NULL) {
		texture->data = NULL;
		texture->impl = NULL;
		texture->size = 0;
		texture->type = GFX_TEXTURE_UNKNOWN;
	}
}

void gfx_texture_destroy(gfx_texture_t* texture)
{
	if (texture != NULL) {
		if (texture->data != NULL) {
			BM_FREE(texture->data);
			texture->data = NULL;
		}
		switch (texture->type) {
		case GFX_TEXTURE_2D:
			gfx_texture2d_destroy(texture);
			break;
		case GFX_TEXTURE_1D:
		case GFX_TEXTURE_3D:
		case GFX_TEXTURE_CUBE:
		default:
			break;
		}
		if (texture->impl != NULL) {
			BM_FREE(texture->impl);
			texture->impl = NULL;
		}
		BM_FREE(texture);
		texture = NULL;
	}
}

result gfx_texture_from_image(struct media_image* img, gfx_texture_t** tex)
{
	if (img == NULL || tex == NULL)
		return RESULT_NULL;
	struct gfx_texture_desc td = {.width = img->width,
				      .height = img->height,
				      .type = GFX_TEXTURE_2D,
				      .pix_fmt = img->pix_fmt,
				      .flags = 0,
				      .mip_levels = 1};
	return gfx_texture_create(img->frame.data[0], &td, tex);
}
