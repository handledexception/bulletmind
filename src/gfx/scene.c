#include "gfx/scene.h"
#include "math/types.h"
#include "core/memory.h"

struct gfx_scene* gfx_scene_new(u32 num_verts, u32 num_indices,
				enum gfx_vertex_type vert_type)
{
	// allocate the scene
	struct gfx_scene* scene = MEM_ALLOC(sizeof(*scene));
	memset(scene, 0, sizeof(*scene));
	// allocate vertex data
	scene->vert_data = MEM_ALLOC(sizeof(*scene->vert_data));
	memset(scene->vert_data, 0, sizeof(*scene->vert_data));
	// allocate index data
	scene->vert_data->type = vert_type;
	scene->index_data = MEM_ALLOC(sizeof(u32) * num_indices);
	memset(scene->index_data, 0, sizeof(u32) * num_indices);

	vec_init(scene->textures);

	// allocate texture vertices
	if (vert_type == GFX_VERTEX_POS_COLOR) {
		scene->vert_data->num_vertices = num_verts;
		scene->vert_data->positions =
			MEM_ALLOC(sizeof(struct vec3f) * num_verts);
		scene->vert_data->colors =
			MEM_ALLOC(sizeof(struct vec4f) * num_verts);
		scene->vert_data->normals = NULL;
		scene->vert_data->tangents = NULL;
		scene->vert_data->tex_verts = NULL;
	} else if (vert_type == GFX_VERTEX_POS_UV) {
		scene->vert_data->num_vertices = num_verts;
		scene->vert_data->positions =
			MEM_ALLOC(sizeof(struct vec3f) * num_verts);
		scene->vert_data->colors = NULL;
		scene->vert_data->normals = NULL;
		scene->vert_data->tangents = NULL;
		scene->vert_data->tex_verts =
			MEM_ALLOC(sizeof(struct texture_vertex) * num_verts);
		size_t uv_size = sizeof(struct vec2f);
		for (size_t i = 0; i < num_verts; i++) {
			scene->vert_data->tex_verts[i].data =
				MEM_ALLOC(uv_size);
			memset(scene->vert_data->tex_verts[i].data, 0,
			       sizeof(uv_size));
			scene->vert_data->tex_verts[i].size = uv_size;
		}
	}
	return scene;
}

void gfx_scene_free(struct gfx_scene* scene)
{
	if (scene) {
		// if (scene->vertex_shader) {
		// 	// scene->vertex_shader->vars
		// 	gfx_shader_free(scene->vertex_shader);
		// 	scene->vertex_shader = NULL;
		// }
		// if (scene->pixel_shader) {
		// 	gfx_shader_free(scene->pixel_shader);
		// 	scene->pixel_shader = NULL;
		// }
		if (scene->vert_data != NULL) {
			if (scene->vert_data->positions != NULL) {
				BM_MEM_FREE(scene->vert_data->positions);
				scene->vert_data->positions = NULL;
			}
			if (scene->vert_data->normals != NULL) {
				BM_MEM_FREE(scene->vert_data->normals);
				scene->vert_data->normals = NULL;
			}
			if (scene->vert_data->tangents != NULL) {
				BM_MEM_FREE(scene->vert_data->tangents);
				scene->vert_data->tangents = NULL;
			}
			if (scene->vert_data->colors != NULL) {
				BM_MEM_FREE(scene->vert_data->colors);
				scene->vert_data->colors = NULL;
			}
			if (scene->vert_data->tex_verts != NULL) {
				for (size_t i = 0;
				     i < scene->vert_data->num_vertices; i++) {
					if (scene->vert_data->tex_verts[i]
						    .data != NULL) {
						BM_MEM_FREE(
							scene->vert_data
								->tex_verts[i]
								.data);
						scene->vert_data->tex_verts[i]
							.data = NULL;
						scene->vert_data->tex_verts[i]
							.size = 0;
					}
				}
				BM_MEM_FREE(scene->vert_data->tex_verts);
				scene->vert_data->tex_verts = NULL;
			}
			BM_MEM_FREE(scene->vert_data);
			scene->vert_data = NULL;
		}
		if (scene->index_data != NULL) {
			BM_MEM_FREE(scene->index_data);
			scene->index_data = NULL;
		}

		for (size_t i = 0; i < scene->textures.num_elems; i++) {
			gfx_texture_destroy(scene->textures.elems[i]);
		}
		vec_free(scene->textures);
	}
	BM_MEM_FREE(scene);
	scene = NULL;
}
