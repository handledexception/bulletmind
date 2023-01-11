#include "core/types.h"
#include "core/asset.h"
#include "core/path.h"
#include "core/logger.h"
#include "core/utils.h"
#include "core/vector.h"
#include "math/types.h"
#include "math/utils.h"
#include "platform/platform.h"
#include "gui/gui.h"
#include "gfx/gfx.h"
#include "gfx/geom.h"
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

static const int kCommandRotateCam = 123;
static const vec3f_t kCameraOrigin = { .x = 0.0f, .y = 0.25f, 0.0f };
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
	camera_t* cam;
	imgui_draw_data_t* ig_draw;
	VECTOR(struct gfx_scene*) scenes;
	gfx_buffer_t* vbuf;
	gfx_buffer_t* ibuf;
	uint64_t frame_count;
	f64 frame_timer;
	f64 start_time;
	f64 end_time;
};

void app_frame_start(struct application* app)
{
	app->start_time = os_get_time_msec();
}

void app_frame_end(struct application* app)
{
	app->end_time = os_get_time_msec() - app->start_time;
}

f64 app_frame_time(struct application* app)
{
	return app->end_time;
}

result app_init_gfx(struct application* app, const struct gfx_config* cfg)
{
	ENSURE_OK(gfx_init(cfg, GFX_D3D11 | GFX_USE_ZBUFFER));

	//TODO: Move sampler state stuff into pixel shader
	struct gfx_sampler_desc sd = {
		.filter = GFX_FILTER_POINT,
		.address_u = GFX_SAMPLE_ADDRESS_CLAMP,
		.address_v = GFX_SAMPLE_ADDRESS_CLAMP,
		.address_w = GFX_SAMPLE_ADDRESS_CLAMP,
		.border_color = {0.f, 0.f, 0.f, 0.f},
		.max_anisotropy = 1,
	};
	gfx_sampler_state_t* sampler = gfx_sampler_state_new(&sd);
	gfx_system_sampler_push(sampler);

	struct gfx_raster_desc rd;
	gfx_raster_desc_init(&rd);
	// rd.culling_mode = GFX_CULLING_FRONT_FACE;
	// rd.winding_order = GFX_WINDING_CCW;
	rd.raster_flags = GFX_RASTER_ANTIALIAS_LINES|GFX_RASTER_MULTI_SAMPLE;//|GFX_RASTER_WIREFRAME;
	ENSURE_OK(gfx_init_rasterizer(&rd));

	rect_t viewport = {.x = 0, .y = 0, .w = cfg->width, .h = cfg->height};
	app->cam = gfx_camera_new();
	camera_t cam = gfx_camera_persp(viewport);
	gfx_camera_set_pos(&cam, vec3_copy(kCameraOrigin));
	gfx_camera_offset_orientation(app->cam, cam.angles.x, cam.angles.y);
	memcpy(app->cam, &cam, sizeof(camera_t));

	gfx_init_cimgui();
	app->ig_draw = malloc(sizeof(imgui_draw_data_t));
	app->ig_draw->view_mat = malloc(sizeof(mat4f_t));
	app->ig_draw->proj_mat = malloc(sizeof(mat4f_t));
	return RESULT_OK;
}

void app_refresh_gfx(struct application* app)
{
	static bool rotate_plane = false;
	f32 rotation_angle = 0.0f;
	static f32 bigbox_angle = 0.0f;
	static const char* bg_want_sprite = "metro";
	if (app->frame_timer == 0.0)
		app->frame_timer = os_get_time_msec();
	if (!app->vbuf || !app->ibuf) {
		logger(LOG_ERROR, "app_refresh_gfx: vertex/index buffer(s) not configured!");
		return;
	}
	u8* vbuf_data = gfx_buffer_get_data(app->vbuf);
	u32* ibuf_data = (u32*)gfx_buffer_get_data(app->ibuf);
	if (!vbuf_data || !ibuf_data) {
		logger(LOG_ERROR, "app_refresh_gfx: vertex/index buffer(s) data not set!");
		return;
	}
	memset(vbuf_data, 0, gfx_buffer_get_size(app->vbuf));
	memset(ibuf_data, 0, gfx_buffer_get_size(app->ibuf));
	gfx_cimgui_begin();
	app->ig_draw->cam = app->cam;
	app->ig_draw->inputs = app->inputs;
	app->ig_draw->frame_count = app->frame_count;
	app->ig_draw->frame_time = app_frame_time(app);
	gfx_cimgui_frame(app->ig_draw);
	gfx_render_clear(&kClearColor);
	static size_t sprite_index = 0;
	size_t vb_data_size = 0;
	size_t vb_data_offs = 0;
	size_t tex_vert_size = 0;
	size_t vert_stride = 0;

	for (size_t sdx = 0; sdx < app->scenes.num_elems; sdx++) {
		vb_data_offs = 0;
		struct gfx_scene* scene = (gfx_scene_t*)app->scenes.elems[sdx];
		if (!scene) {
			logger(LOG_WARNING, "app_refresh_gfx: gfx_scene is NULL!");
			continue;
		}

		// set bg sprite
		if (sdx == app->scenes.num_elems-1) {
			for (size_t adx = 0; adx < scene->assets.num_elems; adx++) {
				asset_t* asset = scene->assets.elems[adx];
				if (!strcmp(asset->name, bg_want_sprite)) {
					scene->curr_sprite = (gfx_sprite_t*)asset->data;
					break;
				}
			}
			if (os_get_time_msec() - app->frame_timer >= 2000.) {
				if (!strcmp(bg_want_sprite, "metro"))
					bg_want_sprite = "rgba_test";
				else if (!strcmp(bg_want_sprite, "rgba_test"))
					bg_want_sprite = "metro";
				app->frame_timer = 0.0;
			}
		}

		gfx_shader_t* vs = scene->curr_vertex_shader;
		gfx_shader_t* ps = scene->curr_pixel_shader;
		if (vs != NULL) {
			if (!strcmp(scene->name, "bigbox") && rotate_plane) {
				if (bigbox_angle >= 360.0f)
					bigbox_angle = 0.0f;
				rotation_angle = bigbox_angle;
				bigbox_angle += 0.01f;
			} else {
				rotation_angle = scene->rot_angle;
			}
			mat4f_t view_mat = gfx_camera_get_view(app->cam);
			memcpy(app->ig_draw->view_mat, &view_mat, sizeof(mat4f_t));
			mat4f_t proj_mat = gfx_camera_get_proj(app->cam);
			memcpy(app->ig_draw->proj_mat, &proj_mat, sizeof(mat4f_t));
			mat4f_t trans_mat = mat4_translate(scene->pos);
			mat4f_t scale_mat = mat4_scale(scene->scale);
			mat4f_t rot_mat = mat4_rotate(rotation_angle, scene->rot_axis);
			mat4f_t world_mat = mat4_transpose(mat4_mul(mat4_mul(mat4_mul(mat4_identity(), rot_mat), trans_mat), scale_mat));
			mat4f_t viewproj_mat = mat4_transpose(gfx_camera_get_view_proj(app->cam));
			gfx_shader_set_var_by_name(vs, "world", &world_mat, false);
			gfx_shader_set_var_by_name(vs, "view_proj", &viewproj_mat, false);
		}
		if (ps != NULL) {
			vec2f_t viewport_res = {VIEW_WIDTH, VIEW_HEIGHT};
			gfx_shader_set_var_by_name(ps, "viewport_res",
						   &viewport_res,
						   false);
			if (scene->curr_sprite) {
				gfx_shader_set_var_by_name(
					ps, "texture", scene->curr_sprite->tex, true);
			}
		}

		// FIXME: need a function to get vertex shader stride from gfx_shader_t*
		vert_stride = gfx_get_vertex_stride(scene->mesh->type);

		// Copy vertex buffer data
		for (u32 vdx = 0; vdx < scene->mesh->num_vertices; vdx++) {
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
				tex_vert_size = tex_vert->size;
				memcpy((void*)&vbuf_data[vb_data_offs],
				       (const void*)tex_vert->data,
				       tex_vert_size);
				vb_data_offs += tex_vert_size;
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
				(tex_vert_size * scene->mesh->num_vertices);
		}

		for (u32 idx = 0; idx < scene->num_indices; idx++) {
			memcpy(&ibuf_data[idx], &scene->index_data[idx],
			       sizeof(u32));
		}

		gfx_set_vertex_shader(vs);
		gfx_set_pixel_shader(ps);
		gfx_system_bind_input_layout(vs);

		f32 cam_speed = 0.000125f;
		vec3f_t cam_vel = { 0.0f, 0.0f, 0.0f };
		inp_cmd_toggle(app->inputs, kCommandRotateCam, &rotate_plane);
		if (inp_cmd_get_state(app->inputs, kCommandMoveForward)) { /* W */
			cam_vel = vec3_add(cam_vel, gfx_camera_forward(app->cam));
		}
		if (inp_cmd_get_state(app->inputs, kCommandMoveBack)) { /* S */
			cam_vel = vec3_add(cam_vel, gfx_camera_backward(app->cam));
		}
		if (inp_cmd_get_state(app->inputs, kCommandMoveLeft)) { /* A */
			cam_vel = vec3_add(cam_vel, gfx_camera_left(app->cam));
		}
		if (inp_cmd_get_state(app->inputs, kCommandMoveRight)) { /* D */
			cam_vel = vec3_add(cam_vel, gfx_camera_right(app->cam));
		}
		if (inp_cmd_get_state(app->inputs, kCommandMoveDown)) { /* Q */
			cam_vel = vec3_mulf(vec3_copy(kVec3Up), cam_speed);
		}
		if (inp_cmd_get_state(app->inputs, kCommandMoveUp)) { /* E */
			cam_vel = vec3_mulf(vec3_copy(kVec3Down), cam_speed);
		}

		gfx_camera_set_pos(app->cam, vec3_add(app->cam->transform.position, vec3_mulf(vec3_norm(cam_vel), app_frame_time(app) * cam_speed)));

		//TODO: Move sampler state into pixel shader, add state param to bind function
		gfx_bind_blend_state();
		gfx_shader_var_t* tex_var =
			gfx_shader_get_var_by_name(ps, "texture");
		if (tex_var)
			gfx_bind_sampler_state((gfx_texture_t*)tex_var->data,
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
		gfx_buffer_copy(app->ibuf, ibuf_data,
				scene->num_indices * sizeof(u32));
		gfx_system_bind_render_target();
		gfx_toggle_depth(true);
		gfx_bind_primitive_topology(GFX_TOPOLOGY_TRIANGLE_LIST);
		gfx_bind_rasterizer();
		gfx_render_begin(true);
	}
	gfx_cimgui_end();
	gfx_render_end(false, 0);
	// os_sleep_ms(16);
	app->frame_count++;
}

result app_init_inputs(struct application* app)
{
	if (app->inputs)
		inp_free(app->inputs);
	app->inputs = inp_new(gui_get_mouse());
	inp_bind_virtual_key(app->inputs, kCommandQuit, SCANCODE_ESCAPE);
	inp_bind_virtual_key(app->inputs, kCommandRotateCam, SCANCODE_F5);
	inp_bind_virtual_key(app->inputs, kCommandMoveForward, SCANCODE_W);
	inp_bind_virtual_key(app->inputs, kCommandMoveBack, SCANCODE_S);
	inp_bind_virtual_key(app->inputs, kCommandMoveLeft, SCANCODE_A);
	inp_bind_virtual_key(app->inputs, kCommandMoveRight, SCANCODE_D);
	inp_bind_virtual_key(app->inputs, kCommandMoveUp, SCANCODE_Q);
	inp_bind_virtual_key(app->inputs, kCommandMoveDown, SCANCODE_E);
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
	size_t index_buffer_size = sizeof(u32) * BM_GFX_MAX_INDICES;
	gfx_buffer_new(NULL, index_buffer_size, GFX_BUFFER_INDEX,
		       GFX_BUFFER_USAGE_DYNAMIC, &app->ibuf);
	logger(LOG_INFO, "Created index buffer (%zu bytes)", index_buffer_size);

	size_t cbuf_size = 0;
	gfx_shader_var_t world_var = {.name = "world",
					.type = GFX_SHADER_VAR_MAT4,
					.data = NULL,
					.own_data = true};
	gfx_shader_var_t view_proj_var = {.name = "view_proj",
					.type = GFX_SHADER_VAR_MAT4,
					.data = NULL,
					.own_data = true};
	gfx_shader_var_t viewport_res_var = {.name = "viewport_res",
						.type = GFX_SHADER_VAR_VEC2,
						.data = NULL,
						.own_data = true};
	// shader vars
	vec2f_t xy_scale = vec2_divf(vec2_set(1.0f, 0.5625f), 4.0f);

	asset_t* vs_asset;
	asset_t* ps_asset;
	ENSURE_OK(asset_manager_find("vs_pos_color", app->assets, &vs_asset));
	ENSURE_OK(asset_manager_find("ps_pos_color", app->assets, &ps_asset));
	gfx_shader_t* vs = (gfx_shader_t*)vs_asset->data;
	gfx_shader_t* ps = (gfx_shader_t*)ps_asset->data;
	gfx_shader_add_var(vs, world_var);
	gfx_shader_add_var(vs, view_proj_var);
	gfx_shader_add_var(ps, viewport_res_var);
	cbuf_size = gfx_shader_get_vars_size(vs);
	gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
			GFX_BUFFER_USAGE_DYNAMIC, &vs->cbuffer);
	cbuf_size = gfx_shader_get_vars_size(ps);
	gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
			GFX_BUFFER_USAGE_DYNAMIC, &ps->cbuffer);

	vec3f_t cube_pos[GFX_CUBE_NUM_VERTS];
	u32 cube_ind[GFX_CUBE_NUM_INDICES];
	vec3f_t cube_sz = { 1.0f, 1.0f, 1.0f };
	gfx_compute_cube(cube_sz, &cube_pos[0], &cube_ind[0], true);
	// reverse_indices_winding(&cube_ind[0], GFX_CUBE_NUM_INDICES);

	int num_rows = 4;
	int num_cols = 4;
	for (size_t row = 0; row < num_rows; row++) {
		for (size_t col = 0; col < num_cols; col++) {
			char scene_name[256];
			sprintf(&scene_name[0], "box_row%d_col%d", row, col);
			struct gfx_scene* box_scene = gfx_scene_new(&scene_name[0]);
			gfx_scene_add_asset(box_scene, vs_asset);
			gfx_scene_add_asset(box_scene, ps_asset);

			struct gfx_mesh box_mesh;
			memset(&box_mesh, 0, sizeof(struct gfx_mesh));
			box_mesh.type = GFX_VERTEX_POS_COLOR;
			box_mesh.num_vertices = GFX_CUBE_NUM_VERTS;
			box_mesh.positions = &cube_pos[0];
			vec4f_t colors[GFX_CUBE_NUM_VERTS];
			for (size_t c = 0; c < GFX_CUBE_NUM_VERTS; c++) {
				colors[c].x = random64(0.1, 1.0);
				colors[c].y = random64(0.1, 1.0);
				colors[c].z = random64(0.1, 1.0);
				colors[c].w = 1.0f;
			}
			box_mesh.colors = &colors[0];
			gfx_scene_set_mesh(box_scene, &box_mesh);
			gfx_scene_set_index_data(box_scene, &cube_ind[0], GFX_CUBE_NUM_INDICES);
			gfx_scene_set_pos(box_scene, vec3_set(
				((float)(col)/4.0f)-xy_scale.x*2,
				1.0f,
				((float)(row)/4.0f)
			));
			gfx_scene_set_rotation(box_scene, 0.0f, vec3_set(0.0f, 0.0f, 0.0f));
			gfx_scene_set_scale(box_scene, vec3_set(0.1f, 0.1f, 0.1f));

			vec_push_back(app->scenes, &box_scene);
		}
	}

	struct gfx_scene* sc = gfx_scene_new("bigbox");
	gfx_scene_add_asset(sc, vs_asset);
	gfx_scene_add_asset(sc, ps_asset);
	struct gfx_mesh m;
	memset(&m, 0, sizeof(struct gfx_mesh));
	m.type = GFX_VERTEX_POS_COLOR;
	m.num_vertices = GFX_CUBE_NUM_VERTS;
	m.positions = &cube_pos[0];
	static vec4f_t vertex_colors[GFX_CUBE_NUM_VERTS];
	for (size_t c = 0; c < GFX_CUBE_NUM_VERTS; c++) {
		vertex_colors[c].x = random64(0.1, 1.0);
		vertex_colors[c].y = random64(0.1, 1.0);
		vertex_colors[c].z = random64(0.1, 1.0);
		vertex_colors[c].w = 1.0f;
	}
	m.colors = &vertex_colors[0];
	gfx_scene_set_mesh(sc, &m);
	gfx_scene_set_index_data(sc, &cube_ind[0], GFX_CUBE_NUM_INDICES);
	gfx_scene_set_pos(sc, vec3_set(0.0f, 0.0f, 0.0f));
	gfx_scene_set_rotation(sc, 45.0f, vec3_set(0.0f, 1.0f, 0.0f));
	gfx_scene_set_scale(sc, vec3_set(5.0f, 0.01f, 5.0f));
	vec_push_back(app->scenes, &sc);

	// asset_t* asset;
	// struct gfx_scene* bg_scene = gfx_scene_new("background");
	// // struct gfx_scene* player_scene = gfx_scene_new("player-sprite");
	// ENSURE_OK(asset_manager_find("metro", app->assets, &asset));
	// gfx_scene_add_asset(bg_scene, asset);
	// ENSURE_OK(asset_manager_find("rgba_test", app->assets, &asset));
	// gfx_scene_add_asset(bg_scene, asset);
	// ENSURE_OK(asset_manager_find("vs_pos_uv", app->assets, &asset));
	// gfx_scene_add_asset(bg_scene, asset);
	// gfx_shader_t* vshader = (gfx_shader_t*)asset->data;
	// ENSURE_OK(asset_manager_find("ps_pos_uv", app->assets, &asset));
	// gfx_scene_add_asset(bg_scene, asset);
	// gfx_shader_t* pshader = (gfx_shader_t*)asset->data;

	// struct gfx_mesh bg_mesh;
	// memset(&bg_mesh, 0, sizeof(struct gfx_mesh));
	// bg_mesh.type = GFX_VERTEX_POS_UV;
	// bg_mesh.num_vertices = GFX_QUAD_NUM_VERTICES;
	// bg_mesh.positions = gfx_quad_positions();
	// vec2f_t* tv_data = gfx_quad_uvs();
	// struct texture_vertex tex_verts[GFX_QUAD_NUM_VERTICES];
	// for (size_t i = 0; i < GFX_QUAD_NUM_VERTICES; i++) {
	// 	tex_verts[i].data = (vec2f_t*)(&tv_data[i]);
	// 	tex_verts[i].size = sizeof(vec2f_t);
	// }
	// bg_mesh.tex_verts = &tex_verts[0];
	// gfx_scene_set_mesh(bg_scene, &bg_mesh);
	// gfx_scene_set_index_data(bg_scene, gfx_quad_indices(), GFX_QUAD_NUM_INDICES);

	// gfx_scene_set_pos(bg_scene, &(vec4f_t){0, 0, 0, 0});
	// gfx_scene_set_rotation(bg_scene, &(vec4f_t){0, 0, 0, 0});
	// gfx_scene_set_scale(bg_scene, &(vec4f_t){xy_scale.x, xy_scale.y, 0.0f, 1.0f});

	// // vertex shader vars
	// gfx_shader_add_var(vshader, world_var);
	// gfx_shader_add_var(vshader, view_proj_var);
	// cbuf_size = gfx_shader_get_vars_size(vshader);
	// gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
	// 	       GFX_BUFFER_USAGE_DYNAMIC, &vshader->cbuffer);
	// // pixel shader vars
	// gfx_shader_var_t texture_var = {.name = "texture",
	// 				.type = GFX_SHADER_VAR_TEX,
	// 				.data = NULL,
	// 				.own_data = false};
	// gfx_shader_add_var(pshader, texture_var);
	// gfx_shader_add_var(pshader, viewport_res_var);
	// cbuf_size = gfx_shader_get_vars_size(pshader);
	// gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
	// 	       GFX_BUFFER_USAGE_DYNAMIC, &pshader->cbuffer);
	// vec_push_back(app->scenes, &bg_scene);
	// vec_push_back(app->scenes, &box_scene);

	// cache shaders, sprites, etc. from scene assets
	for (size_t i = 0; i < app->scenes.num_elems; i++) {
		gfx_scene_t* scene = app->scenes.elems[i];
		if (!scene)
			continue;
		for (size_t adx = 0; adx < scene->assets.num_elems; adx++) {
			asset_t* asset = scene->assets.elems[adx];
			if (!asset)
				continue;
			if (asset->kind == ASSET_SHADER) {
				gfx_shader_t* shader_asset =
					(gfx_shader_t*)asset->data;
				if (shader_asset) {
					if (shader_asset->type == GFX_SHADER_VERTEX)
						scene->curr_vertex_shader = shader_asset;
					else if (shader_asset->type == GFX_SHADER_PIXEL)
						scene->curr_pixel_shader = shader_asset;
				}
			} else if (asset->kind == ASSET_SPRITE) {
				scene->curr_sprite = (gfx_sprite_t*)asset->data;
			}
		}
	}
	// vec2f_t vp_res = {VIEW_WIDTH, VIEW_HEIGHT};
	// gfx_shader_var_set(&viewport_res_var, &vp_res);

	// ENSURE_OK(asset_manager_find("metro", app->assets, &asset));
	// struct gfx_sprite* metro_sprite = (struct gfx_sprite*)asset->data;
	// ENSURE_OK(asset_manager_find("rgba_test", app->assets, &asset));
	// struct gfx_sprite* rgba_sprite = (struct gfx_sprite*)asset->data;
	// ENSURE_OK(asset_manager_find("player", app->assets, &asset));
	// struct gfx_sheet* player_sheet = (struct gfx_sheet*)asset->data;

	// asset_t* vs_asset = NULL;
	// asset_t* ps_asset = NULL;
	// ENSURE_OK(asset_manager_find("vs_pos_uv", app->assets, &vs_asset));
	// ENSURE_OK(asset_manager_find("ps_pos_uv", app->assets, &ps_asset));
	// gfx_shader_t* vshader = (gfx_shader_t*)vs_asset->data;
	// gfx_shader_t* pshader = (gfx_shader_t*)ps_asset->data;

	// // vec_push_back(bg_scene->sprites, &rgba_sprite);
	// // vec_push_back(bg_scene->sprites, &metro_sprite);
	// // borrow ref-counted impls from shader assets
	// gfx_scene_set_vertex_shader(bg_scene, gfx_shader_adopt(vshader));
	// gfx_scene_set_pixel_shader(bg_scene, gfx_shader_adopt(pshader));
	// struct gfx_mesh bg_mesh;
	// memset(&bg_mesh, 0, sizeof(struct gfx_mesh));
	// bg_mesh.type = GFX_VERTEX_POS_UV;
	// bg_mesh.num_vertices = 4;
	// vec3f_t positions[4] = {
	// 	{-1.f, -1.f, 0.f},
	// 	{-1.f, 1.f, 0.f},
	// 	{1.f, 1.f, 0.f},
	// 	{1.f, -1.f, 0.f},
	// };
	// bg_mesh.positions = &positions[0];
	// vec2f_t tv_data[4] = {
	// 	{0.f, 1.f},
	// 	{0.f, 0.f},
	// 	{1.f, 0.f},
	// 	{1.f, 1.f},
	// };
	// struct texture_vertex tex_verts[4];
	// for (size_t i = 0; i < 4; i++) {
	// 	tex_verts[i].data = (vec2f_t*)(&tv_data[i]);
	// 	tex_verts[i].size = sizeof(vec2f_t);
	// }
	// bg_mesh.tex_verts = &tex_verts[0];
	// gfx_scene_set_mesh(bg_scene, &bg_mesh);
	// u32 index_data[6] = {0, 1, 2, 2, 3, 0};
	// gfx_scene_set_index_data(bg_scene, &index_data[0], 6);

	// mat4f_t world_mat;
	// mat4f_t trans_mat, scale_mat;
	// mat4f_identity(&world_mat);
	// mat4f_identity(&trans_mat);
	// mat4f_identity(&scale_mat);
	// const vec4f_t trans_vec = {0.f, 0.f, 0.f, 0.f};
	// const vec4f_t scale_vec = {1.f, 0.5625f, // perspective scaling
	// 			   0.f, 1.f};
	// // const vec4f_t scale_vec = { /* ortho */
	// // 	(float)metro_sprite->img->width -
	// // 	(float)VIEW_WIDTH,
	// // 	(float)metro_sprite->img->height -
	// // 	(float)VIEW_HEIGHT,
	// // 	0.f, 1.f
	// // };
	// mat4f_translate(&trans_mat, &trans_vec);
	// mat4f_scale(&scale_mat, &scale_vec);
	// mat4f_mul(&world_mat, &world_mat, &trans_mat);
	// mat4f_mul(&world_mat, &world_mat, &scale_mat);

	// mat4f_t viewproj_mat;
	// mat4f_identity(&viewproj_mat);
	// mat4f_mul(&viewproj_mat, &app->cam.view_matrix,
	// 	  &app->cam.proj_matrix);
	// mat4f_transpose(&viewproj_mat, &viewproj_mat);
	// gfx_shader_var_t world_var = {.name = "world",
	// 			      .type = GFX_SHADER_VAR_MAT4,
	// 			      .data = NULL,
	// 			      .own_data = true};
	// gfx_shader_var_set(&world_var, &world_mat);
	// gfx_shader_var_t view_proj_var = {.name = "view_proj",
	// 				  .type = GFX_SHADER_VAR_MAT4,
	// 				  .data = NULL,
	// 				  .own_data = true};
	// gfx_shader_var_set(&view_proj_var, &viewproj_mat);
	// gfx_shader_add_var(bg_scene->vertex_shader, world_var);
	// gfx_shader_add_var(bg_scene->vertex_shader, view_proj_var);
	// gfx_shader_var_t texture_var = {.name = "texture",
	// 				.type = GFX_SHADER_VAR_TEX,
	// 				.data = metro_sprite->tex,
	// 				.own_data = false};
	// gfx_shader_add_var(bg_scene->pixel_shader, texture_var);
	// vec2f_t vp_res = {VIEW_WIDTH, VIEW_HEIGHT};
	// gfx_shader_var_t viewport_res_var = {.name = "viewport_res",
	// 				     .type = GFX_SHADER_VAR_VEC2,
	// 				     .data = NULL,
	// 				     .own_data = true};
	// gfx_shader_var_set(&viewport_res_var, &vp_res);
	// gfx_shader_add_var(bg_scene->pixel_shader, viewport_res_var);
	// size_t cbuf_size = gfx_shader_get_vars_size(bg_scene->vertex_shader);
	// gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
	// 	       GFX_BUFFER_USAGE_DYNAMIC,
	// 	       &bg_scene->vertex_shader->cbuffer);
	// cbuf_size = gfx_shader_get_vars_size(bg_scene->pixel_shader);
	// gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
	// 	       GFX_BUFFER_USAGE_DYNAMIC,
	// 	       &bg_scene->pixel_shader->cbuffer);
	// vec_push_back(app->scenes, &bg_scene);

	// PLAYER
	// vec_push_back(player_scene->sprites, &player_sheet->sprite);
	// gfx_scene_set_vertex_shader(player_scene, gfx_shader_adopt(vshader));
	// gfx_scene_set_pixel_shader(player_scene, gfx_shader_adopt(pshader));
	// struct gfx_mesh player_mesh;
	// memset(&player_mesh, 0, sizeof(struct gfx_mesh));
	// player_mesh.type = GFX_VERTEX_POS_UV;
	// player_mesh.num_vertices = 4;
	// player_mesh.positions = &positions[0];
	// float ss_frame_uv_x = 1.f / (float)(player_sheet->num_frames);
	// vec2f_t player_tv_data[4] = {
	// 	{0.f, 1.f},
	// 	{0.f, 0.f},
	// 	{ss_frame_uv_x, 0.f},
	// 	{ss_frame_uv_x, 1.f},
	// };
	// struct texture_vertex player_tex_verts[4];
	// for (size_t i = 0; i < 4; i++) {
	// 	player_tex_verts[i].data = (vec2f_t*)(&player_tv_data[i]);
	// 	player_tex_verts[i].size = sizeof(vec2f_t);
	// }
	// player_mesh.tex_verts = &player_tex_verts[0];
	// gfx_scene_set_mesh(player_scene, &player_mesh);
	// u32 player_index_data[6] = {0, 1, 2, 2, 3, 0};
	// gfx_scene_set_index_data(player_scene, &player_index_data[0], 6);

	// mat4f_t player_world_mat;
	// mat4f_t player_trans_mat, player_scale_mat;
	// mat4f_identity(&player_world_mat);
	// mat4f_identity(&player_trans_mat);
	// mat4f_identity(&player_scale_mat);
	// const vec4f_t player_trans_vec = {0.f, 0.f, 0.f, 0.f};
	// const vec4f_t player_scale_vec = {0.1f, 0.1f, 0.f,
	//   1.f}; // perspective scaling
	// const vec4f_t player_scale_vec = { /* ortho */
	// 	(float)player_sheet->sprite->img->width /
	// 		player_sheet->num_frames,
	// 	(float)player_sheet->sprite->img->height,
	// 	0.f, 1.f};
	// mat4f_translate(&player_trans_mat, &player_trans_vec);
	// mat4f_scale(&player_scale_mat, &player_scale_vec);
	// mat4f_mul(&player_world_mat, &player_world_mat,
	// 	  &player_trans_mat);
	// mat4f_mul(&player_world_mat, &player_world_mat,
	// 	  &player_scale_mat);

	// mat4f_t player_vp_matrix;
	// mat4f_identity(&player_vp_matrix);
	// mat4f_mul(&player_vp_matrix, &app->cam.view_matrix,
	// 	  &app->cam.proj_matrix);
	// mat4f_transpose(&player_vp_matrix, &player_vp_matrix);
	// gfx_shader_var_t player_world_var = {.name = "world",
	// 				     .type = GFX_SHADER_VAR_MAT4,
	// 				     .data = NULL,
	// 				     .own_data = true};
	// gfx_shader_var_set(&player_world_var, &player_world_mat);
	// gfx_shader_var_t player_vp_var = {.name = "view_proj",
	// 				  .type = GFX_SHADER_VAR_MAT4,
	// 				  .data = NULL,
	// 				  .own_data = true};
	// gfx_shader_var_set(&player_vp_var, &player_vp_matrix);
	// gfx_shader_add_var(player_scene->vertex_shader, player_world_var);
	// gfx_shader_add_var(player_scene->vertex_shader, player_vp_var);
	// gfx_shader_var_t player_tex_var = {.name = "texture",
	// 				   .type = GFX_SHADER_VAR_TEX,
	// 				   .data = player_sheet->sprite->tex,
	// 				   .own_data = false};
	// gfx_shader_add_var(player_scene->pixel_shader, player_tex_var);
	// vec2f_t player_vp_res = {VIEW_WIDTH, VIEW_HEIGHT};
	// gfx_shader_var_t player_viewport_res_var = {.name = "viewport_res",
	// 					    .type = GFX_SHADER_VAR_VEC2,
	// 					    .data = NULL,
	// 					    .own_data = true};
	// gfx_shader_var_set(&player_viewport_res_var, &player_vp_res);
	// gfx_shader_add_var(player_scene->pixel_shader, player_viewport_res_var);
	// cbuf_size = gfx_shader_get_vars_size(player_scene->vertex_shader);
	// gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
	// 	       GFX_BUFFER_USAGE_DYNAMIC,
	// 	       &player_scene->vertex_shader->cbuffer);
	// cbuf_size = gfx_shader_get_vars_size(player_scene->pixel_shader);
	// gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
	// 	       GFX_BUFFER_USAGE_DYNAMIC,
	// 	       &player_scene->pixel_shader->cbuffer);
	// vec_push_back(app->scenes, &player_scene);

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
	// mat4f_t grid_world_mat;
	// mat4f_t grid_trans_mat, grid_scale_mat;
	// mat4f_identity(&grid_world_mat);
	// mat4f_identity(&grid_trans_mat);
	// mat4f_identity(&grid_scale_mat);
	// const vec4f_t grid_trans_vec = {0.f, 0.f, 0.f, 0.f};
	// const vec4f_t grid_scale_vec = {1.5f, 0.5625f, // perspective scaling
	// 			   0.f, 1.f};
	// // const vec4f_t grid_scale_vec = { /* ortho */
	// // 	(float)grid_sheet->sprite->img->width /
	// // 		grid_sheet->num_frames,
	// // 	(float)grid_sheet->sprite->img->height,
	// // 	0.f, 1.f};
	// mat4f_translate(&grid_trans_mat, &grid_trans_vec);
	// mat4f_scale(&grid_scale_mat, &grid_scale_vec);
	// mat4f_mul(&grid_world_mat, &grid_world_mat,
	// 	  &grid_trans_mat);
	// mat4f_mul(&grid_world_mat, &grid_world_mat,
	// 	  &grid_scale_mat);

	// mat4f_t grid_vp_matrix;
	// mat4f_identity(&grid_vp_matrix);
	// mat4f_mul(&grid_vp_matrix, &app->cam.view_matrix,
	// 	  &app->cam.proj_matrix);
	// mat4f_transpose(&grid_vp_matrix, &grid_vp_matrix);
	// gfx_shader_var_t grid_world_var = {.name = "world",
	// 				     .type = GFX_SHADER_VAR_MAT4,
	// 				     .data = NULL,
	// 				     .own_data = true};
	// gfx_shader_var_set(&grid_world_var, &grid_world_mat);
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

	size_t vbuf_size = 0;
	for (size_t i = 0; i < app->scenes.num_elems; i++) {
		gfx_scene_t* scene = app->scenes.elems[i];
		if (scene) {
			vbuf_size += gfx_mesh_get_size(scene->mesh);
		}
	}
	gfx_buffer_new(NULL, vbuf_size, GFX_BUFFER_VERTEX,
		       GFX_BUFFER_USAGE_DYNAMIC, &app->vbuf);
	logger(LOG_INFO, "Created vertex buffer (%zu bytes)",
	       vbuf_size);

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
	s32 window_flags = GUI_WINDOW_SHOW | GUI_WINDOW_CAPTURE_MOUSE;
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

	app->start_time = os_get_time_msec();

	return RESULT_OK;
}

void app_refresh(struct application* app)
{
	app->running = true;
	while (app->running) {
		app_frame_start(app);

		gui_refresh();

		mouse_t mouse;
		mouse.mode = MOUSE_MODE_RELATIVE;
		gui_read_mouse_state(&mouse);
		// inp_set_mouse_pos(&app->inputs->mouse, mouse.scr_pos, mouse.scr_delta, mouse.wnd_pos, mouse.wnd_delta);
		// printf("dx: %f dy: %f\n", mouse->scr_delta.x, mouse->scr_delta.y);
		// printf("\rsx: %d sy: %d (sdx: %f sdy: %f) | wx: %d wy: %d (wdx: %f wdy: %f)",
		// 	mouse->scr_pos.x, mouse->scr_pos.y,
		// 	mouse->scr_delta.x, mouse->scr_delta.y,
		// 	mouse->wnd_pos.x, mouse->wnd_pos.y,
		// 	mouse->wnd_delta.x, mouse->wnd_delta.y
		// );

		mouse.scr_delta = vec2_mulf(mouse.scr_delta, 0.25f);

		app->cam->angles.x = mouse.scr_delta.x;
		app->cam->angles.y = mouse.scr_delta.y;

		inp_update_mouse(app->inputs, &mouse);

		gfx_camera_offset_orientation(app->cam, app->cam->angles.x, app->cam->angles.y);

		gui_event_t evt;
		while (gui_event_pop(&evt)) {
			// if (evt.type == GUI_EVENT_MOUSE_MOTION) {
			// 	printf("sx: %d sy: %d | wx: %d wy: %d\n",
			// 	       evt.mouse.scr_pos.x,
			// 	       evt.mouse.scr_pos.y,
			// 	       evt.mouse.wnd_pos.x,
			// 	       evt.mouse.wnd_pos.y);
			// 	inp_set_mouse_pos(&app->inputs->mouse,
			// 		evt.mouse.scr_pos, evt.mouse.wnd_pos);
			// }
			inp_read_gui_event(app->inputs, &evt);
		}
		if (inp_cmd_get_state(app->inputs, kCommandQuit))
			app->running = false;
		// if (inp_cmd_get_state(app->inputs, kCommandPlayerPrimaryFire))
		// 	printf("\nmouse left\n");
		// if (inp_cmd_get_state(app->inputs, kCommandPlayerSpeed))
		// 	printf("\nmouse mid\n");
		// if (inp_cmd_get_state(app->inputs, kCommandPlayerAltFire))
		// 	printf("\nmouse right\n");

		app_refresh_gfx(app);
		app_frame_end(app);
	}
}

void app_shutdown(struct application* app)
{
	if (app != NULL) {
		gfx_camera_free(app->cam);
		free(app->ig_draw->view_mat);
		free(app->ig_draw->proj_mat);
		free(app->ig_draw);

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
	logger(LOG_INFO, "\rRendered %lld frames", app.frame_count);
	return RESULT_OK;
}
