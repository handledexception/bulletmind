#include "game/scene.h"
#include "game/asset.h"
#include "core/memory.h"
#include "math/types.h"

gfx_scene_t* gfx_scene_new(const char* name)
{
	// allocate the scene
	gfx_scene_t* scene = BM_ALLOC(sizeof(*scene));
	gfx_scene_init(scene);
	size_t sz_name = strlen(name);
	if (sz_name > SCENE_NAME_LENGTH - 1)
		sz_name = SCENE_NAME_LENGTH - 1;
	strncpy(&scene->name[0], name, sz_name);
	if (scene->name[sz_name + 1] != '\0')
		scene->name[sz_name + 1] = '\0';

	return scene;
}

void gfx_scene_init(gfx_scene_t* scene)
{
	if (scene) {
		memset(scene, 0, sizeof(*scene));
		vec_init(scene->assets);
		scene->mesh = NULL;
		scene->index_data = NULL;
	}
}

void gfx_scene_free(gfx_scene_t* scene)
{
	if (scene) {
		for (size_t i = 0; i < scene->assets.num_elems; i++) {
			asset_t* ass = scene->assets.elems[i];
			asset_free(ass);
		}
		vec_free(scene->assets);
		gfx_mesh_free(scene->mesh);
		if (scene->index_data != NULL) {
			BM_FREE(scene->index_data);
			scene->index_data = NULL;
		}
	}
	BM_FREE(scene);
	scene = NULL;
}

bool gfx_scene_add_asset(gfx_scene_t* scene, asset_t* asset)
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

bool gfx_scene_remove_asset(gfx_scene_t* scene, const char* name)
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

void gfx_scene_set_pos(gfx_scene_t* scene, vec3f_t pos)
{
	if (scene)
		scene->pos = vec3_copy(pos);
}
void gfx_scene_set_rotation(gfx_scene_t* scene, f32 angle, vec3f_t axis)
{
	if (scene) {
		scene->rot_angle = angle;
		scene->rot_axis = vec3_copy(axis);
	}
}
void gfx_scene_set_scale(gfx_scene_t* scene, vec3f_t scale)
{
	if (scene)
		scene->scale = vec3_copy(scale);
}

void gfx_scene_set_mesh(gfx_scene_t* scene, gfx_mesh_t* mesh)
{
	if (scene && mesh && mesh->num_vertices > 0) {
		scene->mesh = mesh;
	}
}

void gfx_scene_copy_mesh(gfx_scene_t* scene, gfx_mesh_t* mesh)
{
	if (scene && mesh && mesh->num_vertices > 0) {
		scene->mesh = gfx_mesh_new(mesh->type, mesh->num_vertices);
		if (GFX_VERTEX_HAS_POS(mesh->type)) {
			memcpy(scene->mesh->positions, mesh->positions,
			       sizeof(vec3f_t) * mesh->num_vertices);
		}
		if (GFX_VERTEX_HAS_COLOR(mesh->type)) {
			memcpy(scene->mesh->colors, mesh->colors,
			       sizeof(vec4f_t) * mesh->num_vertices);
		}
		if (GFX_VERTEX_HAS_NORMAL(mesh->type)) {
			memcpy(scene->mesh->normals, mesh->normals,
			       sizeof(vec3f_t) * mesh->num_vertices);
		}
		if (GFX_VERTEX_HAS_TANGENT(mesh->type)) {
			memcpy(scene->mesh->tangents, mesh->tangents,
			       sizeof(vec3f_t) * mesh->num_vertices);
		}
		if (GFX_VERTEX_HAS_UV(mesh->type)) {
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

void gfx_scene_set_index_data(gfx_scene_t* scene, u32* data, u32 count)
{
	if (scene && data) {
		scene->index_data = data;
		scene->num_indices = count;
	}
}

void gfx_scene_copy_index_data(gfx_scene_t* scene, u32* data, u32 count)
{
	if (scene->index_data != NULL) {
		BM_FREE(scene->index_data);
		scene->index_data = NULL;
	}
	if (scene->index_data == NULL) {
		scene->index_data = BM_ALLOC(sizeof(u32) * count);
		memcpy(scene->index_data, data, sizeof(u32) * count);
		scene->num_indices = count;
	}
}

gfx_scene_inst_t* gfx_scene_inst_new(const char* name, u32 num_instances)
{
	gfx_scene_inst_t* si = BM_ALLOC(sizeof(*si));
	si->num_instances = num_instances;
	si->scene = gfx_scene_new(name);
	si->transforms = BM_ALLOC(sizeof(mat4f_t) * num_instances);
	return si;
}
void gfx_scene_inst_free(gfx_scene_inst_t* scene)
{
	if (scene != NULL) {
		if (scene->scene != NULL) {
			gfx_scene_free(scene->scene);
			scene->scene = NULL;
		}
		if (scene->transforms != NULL) {
			BM_FREE(scene->transforms);
			scene->transforms = NULL;
		}
		if (scene->vbuf != NULL) {
			gfx_buffer_free(scene->vbuf);
			scene->vbuf = NULL;
		}
		if (scene->ibuf != NULL) {
			gfx_buffer_free(scene->ibuf);
			scene->ibuf = NULL;
		}
		if (scene->vbuf_xform != NULL) {
			gfx_buffer_free(scene->vbuf_xform);
			scene->vbuf_xform = NULL;
		}
		BM_FREE(scene);
		scene = NULL;
	}
}
