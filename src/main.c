#include "core/types.h"
#include "core/asset.h"
#include "core/logger.h"
#include "core/utils.h"
#include "core/vector.h"
#include "math/types.h"
#include "platform/platform.h"
#include "gui/gui.h"
#include "gfx/gfx.h"
#include "gfx/camera.h"
#include "gfx/scene.h"
#include "media/image.h"

#include "command.h"
#include "input.h"

#ifdef BM_WINDOWS
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN
#endif

#define APP_NAME "Bulletmind"
#define APP_VER_MAJ 1
#define APP_VER_MIN 0
#define APP_VER_REV 0
#define APP_ASSETS_TOML_PATH "assets/assets.toml"
#define GFX_ADAPTER_INDEX 0
#define VIEW_WIDTH 1280
#define VIEW_HEIGHT 720

static const rgba_t kClearColor = {
	.r = 0,
	.g = 0,
	.b = 0,
	.a = 255,
};

struct application {
	const char* name;
	u32 version;
	bool running;
	VECTOR(gui_window_t*) windows;
	input_state_t* inputs;
	struct asset_manager* assets;
	camera_t cam;
	VECTOR(struct gfx_scene*) scenes;
	gfx_buffer_t* vbuf;
	gfx_buffer_t* ibuf;
	uint64_t frame_count;
	f64 frame_timer;
};

result app_init_gfx(struct application* app, const struct gfx_config* cfg)
{
	rect_t viewport = {.x = 0, .y = 0, .w = VIEW_WIDTH, .h = VIEW_HEIGHT};
	ENSURE_OK(gfx_init(cfg, GFX_D3D11 | GFX_USE_ZBUFFER));
	struct gfx_raster_state_desc raster_desc = {
		.culling_mode = GFX_CULLING_NONE,
		.winding_order = GFX_WINDING_CW,
		.raster_flags = 0};
	//TODO: Move sampler state stuff into pixel shader
	struct gfx_sampler_desc sampler_desc = {
		.filter = GFX_FILTER_POINT,
		.address_u = GFX_SAMPLE_ADDRESS_CLAMP,
		.address_v = GFX_SAMPLE_ADDRESS_CLAMP,
		.address_w = GFX_SAMPLE_ADDRESS_CLAMP,
		.border_color = {0.f, 0.f, 0.f, 0.f},
		.max_anisotropy = 1,
	};
	gfx_sampler_state_t* sampler = gfx_sampler_state_new(&sampler_desc);
	gfx_system_sampler_push(sampler);
	ENSURE_OK(gfx_init_rasterizer(&raster_desc));
	vec3f_t cam_eye = {0.f, 0.f, -1.f};
	vec3f_t cam_dir = {0.f, 0.f, 1.f};
	vec3f_t cam_up = {0.f, 1.f, 0.f};
	gfx_camera_new(&app->cam);
	// gfx_camera_ortho(&app->cam, &cam_eye, &cam_dir, &cam_up, &viewport, Z_NEAR, Z_FAR);
	gfx_camera_persp(&app->cam, &cam_eye, &cam_dir, &cam_up, &viewport,
			 90.f, Z_NEAR, Z_FAR);

	return RESULT_OK;
}

void app_refresh_gfx(struct application* app)
{
	static size_t sprite_index = 0;
	if (app->frame_timer == 0.0)
		app->frame_timer = os_get_time_msec();
	gfx_render_clear(&kClearColor);
	size_t vb_data_size = 0;
	size_t vb_data_offs = 0;
	size_t tex_vert_size = 0;
	size_t vert_stride = 0;
	u8* vbuf_data = gfx_buffer_get_data(app->vbuf);
	u32* ibuf_data = (u32*)gfx_buffer_get_data(app->ibuf);
	size_t vertex_buffer_size = (BM_GFX_MAX_VERTICES * sizeof(vec3f_t)) +
				    (BM_GFX_MAX_VERTICES * sizeof(vec2f_t));
	size_t index_buffer_size = sizeof(u32) * BM_GFX_MAX_INDICES;
	memset(vbuf_data, 0, vertex_buffer_size);
	memset(ibuf_data, 0, index_buffer_size);
	for (size_t sdx = 0; sdx < app->scenes.num_elems; sdx++) {
		vb_data_offs = 0;
		struct gfx_scene* scene =
			(struct gfx_scene*)app->scenes.elems[sdx];
		gfx_shader_t* vs = scene->vertex_shader;
		gfx_shader_t* ps = scene->pixel_shader;
		vert_stride = gfx_get_vertex_stride(
			scene->mesh
				->type); // FIXME: need a function to get vertex shader stride from gfx_shader_t*
		// Copy vertex buffer data
		for (u32 vdx = 0; vdx < scene->mesh->num_vertices; vdx++) {
			memcpy((void*)&vbuf_data[vb_data_offs],
			       (const void*)&scene->mesh->positions[vdx],
			       sizeof(struct vec3f));
			vb_data_offs += sizeof(struct vec3f);
			struct texture_vertex* tex_vert =
				&scene->mesh->tex_verts[vdx];
			tex_vert_size = tex_vert->size;
			memcpy((void*)&vbuf_data[vb_data_offs],
			       (const void*)tex_vert->data, tex_vert_size);
			vb_data_offs += tex_vert_size;
		}
		vb_data_size += (sizeof(vec3f_t) * scene->mesh->num_vertices) +
				(tex_vert_size * scene->mesh->num_vertices);
		for (u32 idx = 0; idx < 6; idx++) {
			memcpy(&ibuf_data[idx], &scene->index_data[idx],
			       sizeof(u32));
		}

		gfx_set_vertex_shader(vs);
		gfx_set_pixel_shader(ps);
		gfx_system_bind_input_layout(vs);

		if (sdx == 0) {
			gfx_sprite_t* sprite =
				scene->sprites.elems[sprite_index];
			gfx_shader_set_var_by_name(ps, "texture", sprite->tex,
						   0, false);
			if (os_get_time_msec() - app->frame_timer >= 2000.) {
				if (++sprite_index >= scene->sprites.num_elems)
					sprite_index = 0;
				app->frame_timer = 0.0;
			}
		} else {
			gfx_sprite_t* sprite = scene->sprites.elems[0];
			gfx_shader_set_var_by_name(ps, "texture", sprite->tex,
						   0, false);
			gfx_shader_var_t* world_var =
				gfx_shader_get_var_by_name(vs, "world");
			mat4f_t* world_matrix = (mat4f_t*)world_var->data;
			vec4f_t tv = {0};
			if (inp_cmd_get_state(app->inputs, kCommandPlayerLeft))
				tv.x = -0.05f;
			if (inp_cmd_get_state(app->inputs, kCommandPlayerRight))
				tv.x = 0.05f;
			if (inp_cmd_get_state(app->inputs, kCommandPlayerUp))
				tv.y = 0.05f;
			if (inp_cmd_get_state(app->inputs, kCommandPlayerDown))
				tv.y = -0.05f;
			mat4f_t tm;
			mat4f_identity(&tm);
			mat4f_translate(&tm, &tv);
			mat4f_mul(world_matrix, world_matrix, &tm);
		}
		//TODO: Move sampler state into pixel shader, add state param to bind function
		gfx_bind_blend_state();
		gfx_bind_sampler_state((gfx_texture_t*)ps->vars.elems[0].data,
				       0);
		gfx_bind_vertex_buffer(app->vbuf, (u32)vert_stride, 0);
		gfx_bind_index_buffer(app->ibuf, 0);
		if (gfx_shader_cbuffer_fill(vs) > 0) {
			gfx_buffer_upload_constants(vs);
		}
		if (gfx_shader_cbuffer_fill(ps) > 0) {
			gfx_buffer_upload_constants(ps);
		}
		gfx_buffer_copy(app->vbuf, vbuf_data, vb_data_size);
		gfx_buffer_copy(app->ibuf, ibuf_data, 6 * sizeof(u32));
		gfx_system_bind_render_target();
		gfx_toggle_depth(false);
		gfx_bind_primitive_topology(GFX_TOPOLOGY_TRIANGLE_LIST);
		gfx_bind_rasterizer();
		gfx_render_begin(true);
	}
	gfx_render_end(false, 0);
	os_sleep_ms(16);
	app->frame_count++;
}

result app_init_inputs(struct application* app)
{
	if (app->inputs)
		inp_free(app->inputs);
	app->inputs = inp_new();
	inp_bind_virtual_key(app->inputs, kCommandQuit, SCANCODE_ESCAPE);
	inp_bind_virtual_key(app->inputs, kCommandPlayerUp, SCANCODE_W);
	inp_bind_virtual_key(app->inputs, kCommandPlayerDown, SCANCODE_S);
	inp_bind_virtual_key(app->inputs, kCommandPlayerLeft, SCANCODE_A);
	inp_bind_virtual_key(app->inputs, kCommandPlayerRight, SCANCODE_D);
	inp_bind_virtual_key(app->inputs, kCommandPlayerSpeed, SCANCODE_LSHIFT);
	inp_bind_virtual_mouse_button(app->inputs, kCommandPlayerPrimaryFire,
				      MOUSE_BUTTON_LEFT);
	inp_bind_virtual_mouse_button(app->inputs, kCommandPlayerAltFire,
				      MOUSE_BUTTON_RIGHT);
	inp_bind_virtual_mouse_button(app->inputs, kCommandPlayerSpeed,
				      MOUSE_BUTTON_MIDDLE);
	return RESULT_OK;
}

result app_init_assets(struct application* app, const char* assets_toml_path)
{
	if (app->assets)
		asset_manager_free(app->assets);
	app->assets = asset_manager_new();
	return asset_manager_load_toml(assets_toml_path, app->assets);
}

result app_init_scenes(struct application* app)
{
	asset_t* asset;
	ENSURE_OK(asset_manager_find("metro", app->assets, &asset));
	struct gfx_sprite* metro_sprite = (struct gfx_sprite*)asset->data;
	ENSURE_OK(asset_manager_find("rgba_test", app->assets, &asset));
	gfx_shader_var_t rgba_tex_var = {.name = "rgba_test",
					 .type = GFX_SHADER_VAR_TEX,
					 .data = NULL,
					 .own_data = false};
	struct gfx_sprite* rgba_sprite = (struct gfx_sprite*)asset->data;
	ENSURE_OK(asset_manager_find("player", app->assets, &asset));
	struct gfx_sheet* player_sheet = (struct gfx_sheet*)asset->data;

	asset_t* vs_asset = NULL;
	asset_t* ps_asset = NULL;
	ENSURE_OK(asset_manager_find("vs_pos_uv", app->assets, &vs_asset));
	ENSURE_OK(asset_manager_find("ps_pos_uv", app->assets, &ps_asset));
	gfx_shader_t* vshader = (gfx_shader_t*)vs_asset->data;
	gfx_shader_t* pshader = (gfx_shader_t*)ps_asset->data;

	size_t vertex_buffer_size = (BM_GFX_MAX_VERTICES * sizeof(vec3f_t)) +
				    (BM_GFX_MAX_VERTICES * sizeof(vec2f_t));
	gfx_buffer_new(NULL, vertex_buffer_size, GFX_BUFFER_VERTEX,
		       GFX_BUFFER_USAGE_DYNAMIC, &app->vbuf);
	logger(LOG_INFO, "Created vertex buffer (%zu bytes)",
	       vertex_buffer_size);
	size_t index_buffer_size = sizeof(u32) * BM_GFX_MAX_INDICES;
	gfx_buffer_new(NULL, index_buffer_size, GFX_BUFFER_INDEX,
		       GFX_BUFFER_USAGE_DYNAMIC, &app->ibuf);
	logger(LOG_INFO, "Created index buffer (%zu bytes)", index_buffer_size);

	struct gfx_scene* bg_scene = gfx_scene_new("background");
	vec_push_back(bg_scene->sprites, &rgba_sprite);
	vec_push_back(bg_scene->sprites, &metro_sprite);
	// borrow ref-counted impls from shader assets
	gfx_scene_set_vertex_shader(bg_scene, gfx_shader_adopt(vshader));
	gfx_scene_set_pixel_shader(bg_scene, gfx_shader_adopt(pshader));
	struct gfx_mesh bg_mesh;
	memset(&bg_mesh, 0, sizeof(struct gfx_mesh));
	bg_mesh.type = GFX_VERTEX_POS_UV;
	bg_mesh.num_vertices = 4;
	vec3f_t positions[4] = {
		{-1.f, -1.f, 0.f},
		{-1.f, 1.f, 0.f},
		{1.f, 1.f, 0.f},
		{1.f, -1.f, 0.f},
	};
	bg_mesh.positions = &positions[0];
	vec2f_t tv_data[4] = {
		{0.f, 1.f},
		{0.f, 0.f},
		{1.f, 0.f},
		{1.f, 1.f},
	};
	struct texture_vertex tex_verts[4];
	for (size_t i = 0; i < 4; i++) {
		tex_verts[i].data = (vec2f_t*)(&tv_data[i]);
		tex_verts[i].size = sizeof(vec2f_t);
	}
	bg_mesh.tex_verts = &tex_verts[0];
	gfx_scene_set_mesh(bg_scene, &bg_mesh);
	u32 index_data[6] = {0, 1, 2, 2, 3, 0};
	gfx_scene_set_index_data(bg_scene, &index_data[0], 6);

	mat4f_t world_matrix;
	mat4f_t trans_matrix, scale_matrix;
	mat4f_identity(&world_matrix);
	mat4f_identity(&trans_matrix);
	mat4f_identity(&scale_matrix);
	const vec4f_t trans_vec = {0.f, 0.f, 0.f, 0.f};
	const vec4f_t scale_vec = {1.f, 0.5625f, // perspective scaling
				   0.f, 1.f};
	// const vec4f_t scale_vec = { /* ortho */
	// 	(float)metro_sprite->img->width -
	// 	(float)VIEW_WIDTH,
	// 	(float)metro_sprite->img->height -
	// 	(float)VIEW_HEIGHT,
	// 	0.f, 1.f
	// };
	mat4f_translate(&trans_matrix, &trans_vec);
	mat4f_scale(&scale_matrix, &scale_vec);
	mat4f_mul(&world_matrix, &world_matrix, &trans_matrix);
	mat4f_mul(&world_matrix, &world_matrix, &scale_matrix);

	mat4f_t view_proj_matrix;
	mat4f_identity(&view_proj_matrix);
	mat4f_mul(&view_proj_matrix, &app->cam.view_matrix,
		  &app->cam.proj_matrix);
	mat4f_transpose(&view_proj_matrix, &view_proj_matrix);
	gfx_shader_var_t world_var = {.name = "world",
				      .type = GFX_SHADER_VAR_MAT4,
				      .data = NULL,
				      .own_data = true};
	gfx_shader_var_set(&world_var, &world_matrix);
	gfx_shader_var_t view_proj_var = {.name = "view_proj",
					  .type = GFX_SHADER_VAR_MAT4,
					  .data = NULL,
					  .own_data = true};
	gfx_shader_var_set(&view_proj_var, &view_proj_matrix);
	gfx_shader_add_var(bg_scene->vertex_shader, world_var);
	gfx_shader_add_var(bg_scene->vertex_shader, view_proj_var);
	gfx_shader_var_t texture_var = {.name = "texture",
					.type = GFX_SHADER_VAR_TEX,
					.data = metro_sprite->tex,
					.own_data = false};
	gfx_shader_add_var(bg_scene->pixel_shader, texture_var);
	vec2f_t vp_res = {VIEW_WIDTH, VIEW_HEIGHT};
	gfx_shader_var_t viewport_res_var = {.name = "viewport_res",
					     .type = GFX_SHADER_VAR_VEC2,
					     .data = NULL,
					     .own_data = true};
	gfx_shader_var_set(&viewport_res_var, &vp_res);
	gfx_shader_add_var(bg_scene->pixel_shader, viewport_res_var);
	size_t cbuf_size = gfx_shader_get_vars_size(bg_scene->vertex_shader);
	gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
		       GFX_BUFFER_USAGE_DYNAMIC,
		       &bg_scene->vertex_shader->cbuffer);
	cbuf_size = gfx_shader_get_vars_size(bg_scene->pixel_shader);
	gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
		       GFX_BUFFER_USAGE_DYNAMIC,
		       &bg_scene->pixel_shader->cbuffer);
	vec_push_back(app->scenes, &bg_scene);

	// PLAYER
	struct gfx_scene* player_scene = gfx_scene_new("player-sprite");
	vec_push_back(player_scene->sprites, &player_sheet->sprite);
	gfx_scene_set_vertex_shader(player_scene, gfx_shader_adopt(vshader));
	gfx_scene_set_pixel_shader(player_scene, gfx_shader_adopt(pshader));
	struct gfx_mesh player_mesh;
	memset(&player_mesh, 0, sizeof(struct gfx_mesh));
	player_mesh.type = GFX_VERTEX_POS_UV;
	player_mesh.num_vertices = 4;
	player_mesh.positions = &positions[0];
	float ss_frame_uv_x = 1.f / (float)(player_sheet->num_frames);
	vec2f_t player_tv_data[4] = {
		{0.f, 1.f},
		{0.f, 0.f},
		{ss_frame_uv_x, 0.f},
		{ss_frame_uv_x, 1.f},
	};
	struct texture_vertex player_tex_verts[4];
	for (size_t i = 0; i < 4; i++) {
		player_tex_verts[i].data = (vec2f_t*)(&player_tv_data[i]);
		player_tex_verts[i].size = sizeof(vec2f_t);
	}
	player_mesh.tex_verts = &player_tex_verts[0];
	gfx_scene_set_mesh(player_scene, &player_mesh);
	u32 player_index_data[6] = {0, 1, 2, 2, 3, 0};
	gfx_scene_set_index_data(player_scene, &player_index_data[0], 6);

	mat4f_t player_world_matrix;
	mat4f_t player_trans_matrix, player_scale_matrix;
	mat4f_identity(&player_world_matrix);
	mat4f_identity(&player_trans_matrix);
	mat4f_identity(&player_scale_matrix);
	const vec4f_t player_trans_vec = {0.f, 0.f, 0.f, 0.f};
	const vec4f_t player_scale_vec = {0.1f, 0.1f, 0.f,
					  1.f}; // perspective scaling
	// const vec4f_t player_scale_vec = { /* ortho */
	// 	(float)player_sheet->sprite->img->width /
	// 		player_sheet->num_frames,
	// 	(float)player_sheet->sprite->img->height,
	// 	0.f, 1.f};
	mat4f_translate(&player_trans_matrix, &player_trans_vec);
	mat4f_scale(&player_scale_matrix, &player_scale_vec);
	mat4f_mul(&player_world_matrix, &player_world_matrix,
		  &player_trans_matrix);
	mat4f_mul(&player_world_matrix, &player_world_matrix,
		  &player_scale_matrix);

	mat4f_t player_vp_matrix;
	mat4f_identity(&player_vp_matrix);
	mat4f_mul(&player_vp_matrix, &app->cam.view_matrix,
		  &app->cam.proj_matrix);
	mat4f_transpose(&player_vp_matrix, &player_vp_matrix);
	gfx_shader_var_t player_world_var = {.name = "world",
					     .type = GFX_SHADER_VAR_MAT4,
					     .data = NULL,
					     .own_data = true};
	gfx_shader_var_set(&player_world_var, &player_world_matrix);
	gfx_shader_var_t player_vp_var = {.name = "view_proj",
					  .type = GFX_SHADER_VAR_MAT4,
					  .data = NULL,
					  .own_data = true};
	gfx_shader_var_set(&player_vp_var, &player_vp_matrix);
	gfx_shader_add_var(player_scene->vertex_shader, player_world_var);
	gfx_shader_add_var(player_scene->vertex_shader, player_vp_var);
	gfx_shader_var_t player_tex_var = {.name = "texture",
					   .type = GFX_SHADER_VAR_TEX,
					   .data = player_sheet->sprite->tex,
					   .own_data = false};
	gfx_shader_add_var(player_scene->pixel_shader, player_tex_var);
	vec2f_t player_vp_res = {VIEW_WIDTH, VIEW_HEIGHT};
	gfx_shader_var_t player_viewport_res_var = {.name = "viewport_res",
						    .type = GFX_SHADER_VAR_VEC2,
						    .data = NULL,
						    .own_data = true};
	gfx_shader_var_set(&player_viewport_res_var, &player_vp_res);
	gfx_shader_add_var(player_scene->pixel_shader, player_viewport_res_var);
	cbuf_size = gfx_shader_get_vars_size(player_scene->vertex_shader);
	gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
		       GFX_BUFFER_USAGE_DYNAMIC,
		       &player_scene->vertex_shader->cbuffer);
	cbuf_size = gfx_shader_get_vars_size(player_scene->pixel_shader);
	gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
		       GFX_BUFFER_USAGE_DYNAMIC,
		       &player_scene->pixel_shader->cbuffer);
	vec_push_back(app->scenes, &player_scene);

	// GRID
	// int grid_width = 16;
	// int grid_height = 16;
	// gfx_mesh_t* grid_mesh = gfx_mesh_new(GFX_VERTEX_POS_NORM_UV, grid_width * grid_height);
	// struct texture_vertex* tvs = grid_mesh->tex_verts;
	// size_t vcount = 0;
	// for (int y = 0; y < grid_height; y++) {
	// 	float t = (float)y / (float)grid_height;
	// 	for (int x = 0; x < grid_width; x++) {
	// 		float s = (float)x / (float)grid_width;
	// 		vec3f_t pos = { .x = 2 * s - 1.f, .y = 2.f * t - 1.f, .z = 0 };
	// 		memcpy(grid_mesh->positions, &pos, sizeof(vec3f_t));
	// 		grid_mesh->positions++;

	// 		vec2f_t uv = { .x = s, .y = t };
	// 		struct texture_vertex tv;
	// 		tv.data = &uv;
	// 		tv.size = sizeof(vec2f_t);
	// 		memcpy(tvs, &tv, sizeof(struct texture_vertex));
	// 		tvs++;

	// 		vec3f_t norm = { .x = 0, .y = 0, .z = 1 };
	// 		memcpy(grid_mesh->normals, &norm, sizeof(vec3f_t));
	// 		grid_mesh->normals++;

	// 		// if (x < grid_width && y < grid_height) {
	// 		// 	var i = x + y * (grid_width + 1);
	// 		// 	mesh.triangles.push([i, i + 1, i + grid_width + 1]);
	// 		// 	mesh.triangles.push([i + grid_width + 1, i + 1, i + grid_width + 2]);
	// 		// }
	// 		vcount++;
	// 	}
	// }

	// struct gfx_scene* grid_scene = gfx_scene_new("grid");
	// gfx_scene_set_vertex_shader(grid_scene, gfx_shader_adopt(vshader));
	// gfx_scene_set_pixel_shader(grid_scene, gfx_shader_adopt(pshader));
	// gfx_scene_set_mesh(grid_scene, grid_mesh);
	// vec_push_back(grid_scene->sprites, &metro_sprite);
	// u32 grid_index_data[6] = { 0, 1, 2, 2, 3, 0 };
	// gfx_scene_set_index_data(grid_scene, &grid_index_data[0], 6);
	// // gfx_mesh_free(grid_mesh);
	// mat4f_t grid_world_matrix;
	// mat4f_t grid_trans_matrix, grid_scale_matrix;
	// mat4f_identity(&grid_world_matrix);
	// mat4f_identity(&grid_trans_matrix);
	// mat4f_identity(&grid_scale_matrix);
	// const vec4f_t grid_trans_vec = {0.f, 0.f, 0.f, 0.f};
	// const vec4f_t grid_scale_vec = {1.5f, 0.5625f, // perspective scaling
	// 			   0.f, 1.f};
	// // const vec4f_t grid_scale_vec = { /* ortho */
	// // 	(float)grid_sheet->sprite->img->width /
	// // 		grid_sheet->num_frames,
	// // 	(float)grid_sheet->sprite->img->height,
	// // 	0.f, 1.f};
	// mat4f_translate(&grid_trans_matrix, &grid_trans_vec);
	// mat4f_scale(&grid_scale_matrix, &grid_scale_vec);
	// mat4f_mul(&grid_world_matrix, &grid_world_matrix,
	// 	  &grid_trans_matrix);
	// mat4f_mul(&grid_world_matrix, &grid_world_matrix,
	// 	  &grid_scale_matrix);

	// mat4f_t grid_vp_matrix;
	// mat4f_identity(&grid_vp_matrix);
	// mat4f_mul(&grid_vp_matrix, &app->cam.view_matrix,
	// 	  &app->cam.proj_matrix);
	// mat4f_transpose(&grid_vp_matrix, &grid_vp_matrix);
	// gfx_shader_var_t grid_world_var = {.name = "world",
	// 				     .type = GFX_SHADER_VAR_MAT4,
	// 				     .data = NULL,
	// 				     .own_data = true};
	// gfx_shader_var_set(&grid_world_var, &grid_world_matrix);
	// gfx_shader_var_t grid_vp_var = {.name = "view_proj",
	// 				  .type = GFX_SHADER_VAR_MAT4,
	// 				  .data = NULL,
	// 				  .own_data = true};
	// gfx_shader_var_set(&grid_vp_var, &grid_vp_matrix);
	// gfx_shader_add_var(grid_scene->vertex_shader, grid_world_var);
	// gfx_shader_add_var(grid_scene->vertex_shader, grid_vp_var);
	// gfx_shader_var_t grid_tex_var = {.name = "texture",
	// 				   .type = GFX_SHADER_VAR_TEX,
	// 				   .data = metro_sprite->tex,
	// 				   .own_data = false};
	// gfx_shader_add_var(grid_scene->pixel_shader, grid_tex_var);
	// vec2f_t grid_vp_res = {VIEW_WIDTH, VIEW_HEIGHT};
	// gfx_shader_var_t grid_viewport_res_var = {.name = "viewport_res",
	// 					    .type = GFX_SHADER_VAR_VEC2,
	// 					    .data = NULL,
	// 					    .own_data = true};
	// gfx_shader_var_set(&grid_viewport_res_var, &grid_vp_res);
	// gfx_shader_add_var(grid_scene->pixel_shader, grid_viewport_res_var);
	// cbuf_size = gfx_shader_get_vars_size(grid_scene->vertex_shader);
	// gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
	// 	       GFX_BUFFER_USAGE_DYNAMIC,
	// 	       &grid_scene->vertex_shader->cbuffer);
	// cbuf_size = gfx_shader_get_vars_size(grid_scene->pixel_shader);
	// gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
	// 	       GFX_BUFFER_USAGE_DYNAMIC,
	// 	       &grid_scene->pixel_shader->cbuffer);
	// vec_push_back(app->scenes, &grid_scene);

	return RESULT_OK;
}

result app_init(struct application* app, s32 version, u32 vx, u32 vy,
		const char* assets_toml_path)
{
	if (app == NULL)
		return RESULT_NULL;
	app->running = false;
	app->version = version;
	app->frame_count = 0;
	app->frame_timer = 0.0;
	vec_init(app->windows);
	vec_init(app->scenes);

	ENSURE_OK(gui_init());
	char ver_str[16];
	version_string(app->version, ver_str);
	char window_title[256];
	snprintf(window_title, (sizeof(APP_NAME) + 1 + 16) + 1, "%s %s",
		 APP_NAME, ver_str);
	s32 window_flags = GUI_WINDOW_SHOW;
	gui_window_t* main_window =
		gui_create_window(window_title, 0, 0, vx, vy,
				  window_flags | GUI_WINDOW_CENTERED, NULL);
	vec_push_back(app->windows, &main_window);
	gui_window_t* view_window = gui_create_window("canvas_view", 0, 0, vx,
						      vy, window_flags,
						      gui->windows.elems[0]);
	vec_push_back(app->windows, &view_window);
	void* gfx_view_handle = gui_get_window_handle(app->windows.elems[1]);
	const struct gfx_config gfx_cfg = {
		.module = GFX_MODULE_DX11,
		.window = {gfx_view_handle},
		.adapter = GFX_ADAPTER_INDEX,
		.width = vx,
		.height = vy,
		.fps_num = 144,
		.fps_den = 1,
		.fullscreen = false,
		.pix_fmt = PIX_FMT_RGBA32,
	};
	ENSURE_OK(app_init_gfx(app, &gfx_cfg));
	ENSURE_OK(app_init_inputs(app));
	ENSURE_OK(app_init_assets(app, assets_toml_path));
	ENSURE_OK(app_init_scenes(app));

	return RESULT_OK;
}

void app_refresh(struct application* app)
{
	app->running = true;
	while (app->running) {
		gui_refresh();

		mouse_t mouse;
		gui_get_global_mouse_state(&mouse);
		// printf("\rsx: %d sy: %d | wx: %d wy: %d",
		// 	mouse.screen_pos.x, mouse.screen_pos.y,
		// 	mouse.window_pos.x, mouse.window_pos.y);
		inp_set_mouse_pos(&app->inputs->mouse, mouse.screen_pos,
				  mouse.window_pos);

		gui_event_t evt;
		while (gui_poll_event(&evt)) {
			// if (evt.type == GUI_EVENT_MOUSE_MOTION) {
			// 	printf("sx: %d sy: %d | wx: %d wy: %d\n",
			// 	       evt.mouse.screen_pos.x,
			// 	       evt.mouse.screen_pos.y,
			// 	       evt.mouse.window_pos.x,
			// 	       evt.mouse.window_pos.y);
			// 	inp_set_mouse_pos(&app->inputs->mouse,
			// 		evt.mouse.screen_pos, evt.mouse.window_pos);
			// }
			inp_refresh_pressed(app->inputs, &evt);
		}
		if (inp_cmd_get_state(app->inputs, kCommandQuit))
			app->running = false;
		if (inp_cmd_get_state(app->inputs, kCommandPlayerPrimaryFire))
			printf("mouse left\n");
		if (inp_cmd_get_state(app->inputs, kCommandPlayerSpeed))
			printf("mouse mid\n");
		if (inp_cmd_get_state(app->inputs, kCommandPlayerAltFire))
			printf("mouse right\n");
		app_refresh_gfx(app);
	}
}

void app_shutdown(struct application* app)
{
	if (app != NULL) {
		gfx_buffer_free(app->ibuf);
		gfx_buffer_free(app->vbuf);
		for (size_t i = 0; i < app->scenes.num_elems; i++) {
			struct gfx_scene* scene = app->scenes.elems[i];
			gfx_scene_free(app->scenes.elems[i]);
		}
		vec_free(app->scenes);
		asset_manager_free(app->assets);
		inp_free(app->inputs);
		gfx_shutdown();
		gui_shutdown();
		vec_free(app->windows);
	}
}

// #if defined(BM_WINDOWS) && !defined(BM_DEBUG)
// int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
// #else
// #endif
int main(int argc, char** argv)
{
	logger(LOG_INFO, "%s %d.%d.%d (built %s %s)", APP_NAME, APP_VER_MAJ,
	       APP_VER_MIN, APP_VER_REV, BM_BUILD_DATE, BM_BUILD_TIME);
	logger(LOG_INFO, "Platform: %s", os_platform_string());
	logger(LOG_INFO, "Architecture: %s (%s)", os_architecture_string(),
	       os_bits_string());
	logger(LOG_INFO, "Compiler: %s", os_compiler_string());
	logger(LOG_INFO, "Pointer size: %zu bytes", sizeof(intptr_t));
	struct application app;
	memset(&app, 0, sizeof(struct application));
	const u32 app_ver = pack_version(APP_VER_MAJ, APP_VER_MIN, APP_VER_REV);
	ENSURE_OK(app_init(&app, app_ver, VIEW_WIDTH, VIEW_HEIGHT,
			   APP_ASSETS_TOML_PATH));
	app_refresh(&app);
	app_shutdown(&app);
	ENSURE_OK(mem_report_leaks());
	return RESULT_OK;
}
