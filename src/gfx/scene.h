#ifndef H_BM_GFX_SCENE
#define H_BM_GFX_SCENE

#include "gfx/gfx.h"
#include "core/vector.h"

typedef struct gfx_shader gfx_shader_t;
typedef struct asset asset_t;

struct gfx_scene {
	gfx_shader_t* pixel_shader;
	gfx_shader_t* vertex_shader;
	struct gfx_vertex_data* vert_data;
	u32* index_data;
	VECTOR(gfx_sprite_t*) sprites;
};

BM_EXPORT struct gfx_scene* gfx_scene_new(u32 num_verts, u32 num_indices,
					  enum gfx_vertex_type vert_type);
BM_EXPORT void gfx_scene_free(struct gfx_scene* scene);

#endif
