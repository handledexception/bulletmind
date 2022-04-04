#include "gfx/scene.h"
#include "math/types.h"
#include "core/memory.h"

struct gfx_scene* gfx_scene_new(u32 num_verts, u32 num_vars,
				enum gfx_vertex_type vert_type)
{
	struct gfx_scene* scene = mem_alloc(sizeof(*scene));
	scene->vert_data = mem_alloc(sizeof(*scene->vert_data));
	scene->vert_data->type = vert_type;
	if (vert_type == GFX_VERTEX_POS_COLOR) {
		scene->vert_data->num_vertices = num_verts;
		scene->vert_data->positions =
			mem_alloc(sizeof(struct vec3f) * num_verts);
		scene->vert_data->colors =
			mem_alloc(sizeof(struct vec4f) * num_verts);
		scene->vert_data->normals = NULL;
		scene->vert_data->tangents = NULL;
		scene->vert_data->tex_verts = NULL;
	} else if (vert_type == GFX_VERTEX_POS_UV) {
		scene->vert_data->num_vertices = num_verts;
		scene->vert_data->positions =
			mem_alloc(sizeof(struct vec3f) * num_verts);
		scene->vert_data->colors = NULL;
		scene->vert_data->normals = NULL;
		scene->vert_data->tangents = NULL;
		scene->vert_data->tex_verts = mem_alloc(
			sizeof(*scene->vert_data->tex_verts) * num_verts);
		for (size_t i = 0; i < num_verts; i++) {
			size_t data_size = sizeof(struct vec2f);
			scene->vert_data->tex_verts->data =
				mem_alloc(data_size);
			scene->vert_data->tex_verts->size = data_size;
		}
	}
	return scene;
}

void gfx_scene_free(struct gfx_scene* scene)
{
	if (scene) {
		if (scene->vert_data) {
			if (scene->vert_data->positions) {
				mem_free(scene->vert_data->positions);
				scene->vert_data->positions = NULL;
			}
			if (scene->vert_data->normals) {
				mem_free(scene->vert_data->normals);
				scene->vert_data->normals = NULL;
			}
			if (scene->vert_data->tangents) {
				mem_free(scene->vert_data->tangents);
				scene->vert_data->tangents = NULL;
			}
			if (scene->vert_data->colors) {
				mem_free(scene->vert_data->colors);
				scene->vert_data->colors = NULL;
			}
			if (scene->vert_data->tex_verts) {
				if (scene->vert_data->tex_verts->data) {
					mem_free(scene->vert_data->tex_verts
							 ->data);
					scene->vert_data->tex_verts->data =
						NULL;
				}
				mem_free(scene->vert_data->tex_verts);
				scene->vert_data->tex_verts = NULL;
			}
		}
	}
	mem_free(scene);
	scene = NULL;
}
