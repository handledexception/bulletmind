#ifndef H_BM_GFX_SCENE
#define H_BM_GFX_SCENE

#include "gfx/gfx.h"
#include "core/vector.h"

#define SCENE_NAME_LENGTH 256

typedef struct gfx_shader gfx_shader_t;
typedef struct asset asset_t;

typedef struct gfx_scene {
	char name[SCENE_NAME_LENGTH];
	vec4f_t pos;
	vec4f_t rot;
	vec4f_t scale;
	size_t num_indices;
	u32* index_data;
	struct gfx_mesh* mesh;
	gfx_shader_t* curr_vertex_shader;
	gfx_shader_t* curr_pixel_shader;
	gfx_sprite_t* curr_sprite;
	VECTOR(asset_t*) assets;
} gfx_scene_t;

BM_EXPORT struct gfx_scene* gfx_scene_new(const char* name);
BM_EXPORT void gfx_scene_init(gfx_scene_t* scene);
BM_EXPORT void gfx_scene_free(gfx_scene_t* scene);
BM_EXPORT bool gfx_scene_add_asset(gfx_scene_t* scene, asset_t* asset);
BM_EXPORT bool gfx_scene_remove_asset(gfx_scene_t* scene, const char* name);
BM_EXPORT void gfx_scene_set_pos(gfx_scene_t* scene, vec4f_t* pos);
BM_EXPORT void gfx_scene_set_rotation(gfx_scene_t* scene, vec4f_t* rot);
BM_EXPORT void gfx_scene_set_scale(gfx_scene_t* scene, vec4f_t* scale);
BM_EXPORT void gfx_scene_set_mesh(gfx_scene_t* scene, struct gfx_mesh* mesh);
BM_EXPORT void gfx_scene_set_index_data(gfx_scene_t* scene, u32* data,
					u32 count);

#endif
