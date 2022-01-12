#include "gfx/scene.h"
#include "math/types.h"
#include "core/memory.h"

struct gfx_scene* gfx_scene_new(u32 num_verts, u32 num_vars, enum gfx_vertex_type vert_type)
{
    struct gfx_scene* scene = bm_malloc(sizeof(*scene));
    vector_init(&scene->shader_vars);
    scene->vert_data = bm_malloc(sizeof(*scene->vert_data));
    scene->vert_data->type = vert_type;
    if (vert_type == GFX_VERTEX_POS_COLOR) {
        scene->vert_data->num_vertices = num_verts;
        scene->vert_data->positions = bm_malloc(sizeof(struct vec3f) * num_verts);
        scene->vert_data->colors = bm_malloc(sizeof(struct vec4f) * num_verts);
        scene->vert_data->normals = NULL;
        scene->vert_data->tangents = NULL;
        scene->vert_data->tex_verts = NULL;
    } else if (vert_type == GFX_VERTEX_POS_UV) {
        scene->vert_data->num_vertices = num_verts;
        scene->vert_data->positions = bm_malloc(sizeof(struct vec3f) * num_verts);
        scene->vert_data->colors = NULL;
        scene->vert_data->normals = NULL;
        scene->vert_data->tangents = NULL;
        scene->vert_data->tex_verts = bm_malloc(sizeof(*scene->vert_data->tex_verts) * num_verts);
        for (size_t i = 0; i < num_verts; i++) {
            size_t data_size = sizeof(struct vec2f);
            scene->vert_data->tex_verts->data = bm_malloc(data_size);
            scene->vert_data->tex_verts->size = data_size;
        }
    }
    return scene;
}

void gfx_scene_add_shader_var(struct gfx_scene* scene, gfx_shader_var_t* var)
{
    if (scene) {
        vector_push_back(&scene->shader_vars, var, sizeof(gfx_shader_var_t));
    }
}

void gfx_scene_free(struct gfx_scene* scene)
{
    if (scene) {
        vector_free(&scene->shader_vars);
        if (scene->vert_data) {
            if (scene->vert_data->positions) {
                bm_free(scene->vert_data->positions);
                scene->vert_data->positions = NULL;
            }
	        if (scene->vert_data->normals) {
                bm_free(scene->vert_data->normals);
                scene->vert_data->normals = NULL;
            }
	        if (scene->vert_data->tangents) {
                bm_free(scene->vert_data->tangents);
                scene->vert_data->tangents = NULL;
            }
	        if (scene->vert_data->colors) {
                bm_free(scene->vert_data->colors);
                scene->vert_data->colors = NULL;
            }
	        if (scene->vert_data->tex_verts) {
                if (scene->vert_data->tex_verts->data) {
                    bm_free(scene->vert_data->tex_verts->data);
                    scene->vert_data->tex_verts->data = NULL;
                }
                bm_free(scene->vert_data->tex_verts);
                scene->vert_data->tex_verts = NULL;
            }
        }
    }
    bm_free(scene);
    scene = NULL;
}
