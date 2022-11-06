#ifndef H_BM_GFX_SCENE
#define H_BM_GFX_SCENE

#include "gfx/gfx.h"
#include "core/vector.h"

#define SCENE_NAME_LENGTH 256

typedef struct gfx_shader gfx_shader_t;
typedef struct asset asset_t;

struct gfx_scene {
	gfx_shader_t* pixel_shader;
	gfx_shader_t* vertex_shader;
	struct gfx_mesh* mesh;
	u32* index_data;
	VECTOR(asset_t*) assets;
	VECTOR(gfx_sprite_t*) sprites;
	char name[SCENE_NAME_LENGTH + 1];
};

BM_EXPORT struct gfx_scene* gfx_scene_new(const char* name);
BM_EXPORT void gfx_scene_init(struct gfx_scene* scene);
BM_EXPORT void gfx_scene_free(struct gfx_scene* scene);
BM_EXPORT bool gfx_scene_add_asset(struct gfx_scene* scene, asset_t* asset);
BM_EXPORT bool gfx_scene_remove_asset(struct gfx_scene* scene,
				      const char* name);
BM_EXPORT void gfx_scene_set_mesh(struct gfx_scene* scene,
				  struct gfx_mesh* mesh);
BM_EXPORT void gfx_scene_set_index_data(struct gfx_scene* scene, u32* data,
					u32 count);
BM_EXPORT void gfx_scene_set_vertex_shader(struct gfx_scene* scene,
					   gfx_shader_t* vs);
BM_EXPORT void gfx_scene_set_pixel_shader(struct gfx_scene* scene,
					  gfx_shader_t* ps);

#endif
