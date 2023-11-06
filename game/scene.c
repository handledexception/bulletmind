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

void gfx_scene_inst_copy_buffers(gfx_scene_inst_t* si)
{
	gfx_scene_t* scene = si->scene;

	u8* vbuf_data = gfx_buffer_get_data(si->vbuf);
	u32* ibuf_data = (u32*)gfx_buffer_get_data(si->ibuf);
	if (!vbuf_data || !ibuf_data) {
		logger(LOG_ERROR, "app_refresh_gfx: vertex/index buffer(s) data not set!");
		return;
	}
	memset(vbuf_data, 0, gfx_buffer_get_size(si->vbuf));
	memset(ibuf_data, 0, gfx_buffer_get_size(si->ibuf));

	size_t vb_data_size = 0;
	size_t vb_data_offs = 0;
	size_t tv_data_size = 0;
	size_t stride = gfx_get_vertex_stride(scene->mesh->type);

	for (u32 vdx = 0; vdx < si->scene->mesh->num_vertices; vdx++) {
		if (GFX_VERTEX_HAS_POS(scene->mesh->type)) {
			memcpy((void*)&vbuf_data[vb_data_offs],
					(const void*)&scene->mesh->positions[vdx],
					sizeof(vec3f_t));
			vb_data_offs += sizeof(vec3f_t);
		}
		if (GFX_VERTEX_HAS_COLOR(scene->mesh->type)) {
			memcpy((void*)&vbuf_data[vb_data_offs],
					(const void*)&scene->mesh->colors[vdx],
					sizeof(vec4f_t));
			vb_data_offs += sizeof(vec4f_t);
		}
		if (GFX_VERTEX_HAS_UV(scene->mesh->type)) {
			struct texture_vertex* tex_vert =
				&scene->mesh->tex_verts[vdx];
			tv_data_size = tex_vert->size;
			memcpy((void*)&vbuf_data[vb_data_offs],
					(const void*)tex_vert->data,
					tv_data_size);
			vb_data_offs += tv_data_size;
		}
	}
	if (GFX_VERTEX_HAS_POS(scene->mesh->type)) {
		vb_data_size +=
			(sizeof(vec3f_t) * scene->mesh->num_vertices);
	}
	if (GFX_VERTEX_HAS_COLOR(scene->mesh->type)) {
		vb_data_size +=
			(sizeof(vec4f_t) * scene->mesh->num_vertices);
	}
	if (GFX_VERTEX_HAS_UV(scene->mesh->type)) {
		vb_data_size +=
			(tv_data_size * scene->mesh->num_vertices);
	}

	for (u32 idx = 0; idx < scene->mesh->num_indices; idx++) {
		memcpy(&ibuf_data[idx], &scene->mesh->indices[idx],
				sizeof(u32));
	}

	gfx_buffer_copy(si->vbuf, vbuf_data, vb_data_size);
	gfx_buffer_copy(si->ibuf, ibuf_data,
			scene->mesh->num_indices * sizeof(u32));
}
