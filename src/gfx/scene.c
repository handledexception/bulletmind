#include "gfx/scene.h"
#include "math/types.h"
#include "core/asset.h"
#include "core/memory.h"

struct gfx_scene* gfx_scene_new(const char* name)
{
	// allocate the scene
	struct gfx_scene* scene = BM_ALLOC(sizeof(*scene));
	gfx_scene_init(scene);
	size_t sz_name = strlen(name);
	if (sz_name > SCENE_NAME_LENGTH)
		sz_name = SCENE_NAME_LENGTH;
	strncpy(&scene->name[0], name, sz_name + 1);
	if (scene->name[SCENE_NAME_LENGTH] != '\0')
		scene->name[SCENE_NAME_LENGTH] = '\0';

	return scene;
}

void gfx_scene_init(struct gfx_scene* scene)
{
	if (scene) {
		memset(scene, 0, sizeof(*scene));
		vec_init(scene->sprites);
		vec_init(scene->assets);
	}
}

void gfx_scene_free(struct gfx_scene* scene)
{
	if (scene) {
		vec_free(scene->sprites);
		vec_free(scene->assets);
		gfx_mesh_free(scene->mesh);
		if (scene->index_data != NULL) {
			BM_FREE(scene->index_data);
			scene->index_data = NULL;
		}
		if (scene->vertex_shader) {
			gfx_shader_free(scene->vertex_shader);
			scene->vertex_shader = NULL;
		}
		if (scene->pixel_shader) {
			gfx_shader_free(scene->pixel_shader);
			scene->pixel_shader = NULL;
		}
	}
	BM_FREE(scene);
	scene = NULL;
}

bool gfx_scene_add_asset(struct gfx_scene* scene, asset_t* asset)
{
	if (scene && asset) {
		bool found = false;
		for (size_t i = 0; i < scene->assets.num_elems; i++) {
			asset_t* ass = scene->assets.elems[i];
			if (ass && !strcmp(ass->name, asset->name)) {
				found = true;
				break;
			}
		}
		if (!found) {
			asset_acquire(asset);
			vec_push_back(scene->assets, &asset);
			return true;
		}
	}

	return false;
}

bool gfx_scene_remove_asset(struct gfx_scene* scene, const char* name)
{
	if (scene && name) {
		bool found = false;
		size_t i = 0;
		for (i = 0; i < scene->assets.num_elems; i++) {
			asset_t* ass = scene->assets.elems[i];
			if (ass && !strcmp(ass->name, name)) {
				found = true;
				break;
			}
		}
		if (found) {
			asset_t* ass = scene->assets.elems[i];
			asset_release(ass);
			vec_erase(scene->assets, i);
			return true;
		}
	}

	return false;
}

void gfx_scene_set_mesh(struct gfx_scene* scene, struct gfx_mesh* mesh)
{
	if (scene && mesh && mesh->num_vertices > 0) {
		scene->mesh = gfx_mesh_new(mesh->type, mesh->num_vertices);
		if (VERTEX_HAS_POS(mesh->type)) {
			memcpy(scene->mesh->positions, mesh->positions,
			       sizeof(vec3f_t) * mesh->num_vertices);
		}
		if (VERTEX_HAS_COLOR(mesh->type)) {
			memcpy(scene->mesh->colors, mesh->colors,
			       sizeof(vec4f_t) * mesh->num_vertices);
		}
		if (VERTEX_HAS_NORMAL(mesh->type)) {
			memcpy(scene->mesh->normals, mesh->normals,
			       sizeof(vec3f_t) * mesh->num_vertices);
		}
		if (VERTEX_HAS_TANGENT(mesh->type)) {
			memcpy(scene->mesh->tangents, mesh->tangents,
			       sizeof(vec3f_t) * mesh->num_vertices);
		}
		if (VERTEX_HAS_UV(mesh->type)) {
			for (size_t i = 0; i < mesh->num_vertices; i++) {
				scene->mesh->tex_verts[i].size =
					mesh->tex_verts[i].size;
				memcpy(scene->mesh->tex_verts[i].data,
				       mesh->tex_verts[i].data,
				       sizeof(vec2f_t));
			}
		}
	}
}

void gfx_scene_set_index_data(struct gfx_scene* scene, u32* data, u32 count)
{
	scene->index_data = BM_ALLOC(sizeof(u32) * count);
	memcpy(scene->index_data, data, sizeof(u32) * count);
}

void gfx_scene_set_vertex_shader(struct gfx_scene* scene, gfx_shader_t* vs)
{
	if (scene)
		scene->vertex_shader = vs;
}

void gfx_scene_set_pixel_shader(struct gfx_scene* scene, gfx_shader_t* ps)
{
	if (scene)
		scene->pixel_shader = ps;
}
