#pragma once

#include "gfx/gfx.h"
#include "core/vector.h"

#define SCENE_NAME_LENGTH 256

typedef struct gfx_buffer gfx_buffer_t;
typedef struct gfx_shader gfx_shader_t;
typedef struct asset asset_t;

typedef struct gfx_scene {
	char name[SCENE_NAME_LENGTH];
	vec3f_t pos;
	vec3f_t scale;
	vec3f_t rot_axis;
	f32 rot_angle;
	gfx_mesh_t* mesh;
	gfx_shader_t* curr_vertex_shader;
	gfx_shader_t* curr_pixel_shader;
	gfx_sprite_t* curr_sprite;
	VECTOR(asset_t*) assets;
} gfx_scene_t;

BM_EXPORT gfx_scene_t* gfx_scene_new(const char* name);
BM_EXPORT void gfx_scene_init(gfx_scene_t* scene);
BM_EXPORT void gfx_scene_free(gfx_scene_t* scene);
BM_EXPORT bool gfx_scene_add_asset(gfx_scene_t* scene, asset_t* asset);
BM_EXPORT bool gfx_scene_remove_asset(gfx_scene_t* scene, const char* name);
BM_EXPORT void gfx_scene_set_pos(gfx_scene_t* scene, vec3f_t pos);
BM_EXPORT void gfx_scene_set_rotation(gfx_scene_t* scene, f32 angle,
				      vec3f_t axis);
BM_EXPORT void gfx_scene_set_scale(gfx_scene_t* scene, vec3f_t scale);
BM_EXPORT void gfx_scene_set_mesh(gfx_scene_t* scene, gfx_mesh_t* mesh);

typedef struct gfx_scene_inst {
	u32 num_instances;
	mat4f_t* transforms;
	gfx_scene_t* scene;
	gfx_buffer_t* vbuf;
	gfx_buffer_t* ibuf;
	gfx_buffer_t* vbuf_xform;
} gfx_scene_inst_t;

BM_EXPORT gfx_scene_inst_t* gfx_scene_inst_new(const char* name,
					       u32 num_instances);
BM_EXPORT void gfx_scene_inst_free(gfx_scene_inst_t* scene);
BM_EXPORT void gfx_scene_inst_copy_buffers(gfx_scene_inst_t* si);
