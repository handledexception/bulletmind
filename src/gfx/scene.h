#ifndef H_BM_GFX_SCENE
#define H_BM_GFX_SCENE

#include "gfx/gfx.h"
#include "core/vector.h"

struct gfx_scene {
	struct gfx_vertex_data* vert_data;
};

BM_EXPORT struct gfx_scene* gfx_scene_new(u32 num_verts, u32 num_vars,
					  enum gfx_vertex_type vert_type);
BM_EXPORT void gfx_scene_free(struct gfx_scene* scene);

#endif
