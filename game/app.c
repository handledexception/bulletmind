#include "game/app.h"
#include "game/asset.h"
#include "game/command.h"
#include "game/entity.h"
#include "game/scene.h"

#include "platform/input.h"
#include "platform/platform.h"

#include "core/buffer.h"
#include "core/file_io.h"
#include "core/logger.h"
#include "core/path.h"
#include "core/string.h"
#include "core/types.h"
#include "core/utils.h"
#include "core/vector.h"

#include "gfx/gfx.h"
#include "gfx/geometry.h"
#include "gfx/camera.h"

#include "gui/gui.h"

#include "math/types.h"
#include "math/utils.h"

#include "media/image.h"

#define GFX_ADAPTER_INDEX 0
#define FPS_NUM 60
#define FPS_DEN 1
static const f32     kDefaultMouseSensitivity = 0.075f;
static const f32     kFriction 				  = 0.025f;
static const f32     kPlayerSpeed 			  = 0.8f;
static const int     kCommandRotateCam 		  = 100;
static const int     kCommandToggleMouse 	  = 101;
static const int     kCommandToggleFlyCam 	  = 102;
static const vec3f_t kWorldOrigin  = { .x = 0.f,  .y = 0.0f, .z = 0.0f };
static const vec3f_t kCameraOrigin = { .x = 0.0f, .y = 2.0f, .z = 0.0f };
static const rgba_t  kClearColor   = { .r = 0,    .g = 0,    .b = 0, .a = 255 };
static const u32 	 kDefaultHashmapSeed = 0x1337c0d3;

void mat4_printf(mat4f_t m)
{
	printf("%.3f %.3f %.3f %.3f\n%.3f %.3f %.3f %.3f\n%.3f %.3f %.3f %.3f\n%.3f %.3f %.3f %.3f\n",
		m.x.x, m.x.y, m.x.z, m.x.w,
		m.y.x, m.y.y, m.y.z, m.y.w,
		m.z.x, m.z.y, m.z.z, m.z.w,
		m.w.x, m.w.y, m.w.z, m.w.w
	);
}

u32 app_scene_instance_count(struct application* app)
{
	if (app == NULL)
		return 0;
	u32 instances = 0;
	for (int i = 0; i < app->scene_insts.num_elems; i++) {
		gfx_scene_inst_t* inst = app->scene_insts.elems[i];
		if (inst != NULL) {
			instances += inst->num_instances;
		}
	}
	return instances;
}

result app_init(struct application* app, const char* name, u32 version, u32 vx, u32 vy,
		const char* assets_toml_path)
{
	if (app == NULL)
		return RESULT_NULL;
	app->version = version;
	app->fps.num = FPS_NUM;
	app->fps.den = FPS_DEN;
	app->fps_frame_time = app->fps.den / app->fps.num;
	app->time_now = 0.0;
	app->time_delta = 0.0;
	app->frame_count = 0;
	app->frame_timer = 0.0;
	vec_init(app->scenes);
	vec_init(app->scene_insts);

	ENSURE_OK(gui_init(GUI_BACKEND_SDL2));
	char ver_str[16];
	version_string(app->version, ver_str);
	char window_title[256];
	snprintf(window_title, (sizeof(name) + 1 + 16) + 1, "%s %s",
		 name, ver_str);
	s32 window_flags = GUI_WINDOW_SHOW | GUI_WINDOW_CENTERED | GUI_WINDOW_CAPTURE_MOUSE;
	app->window = gui_create_window(window_title, 0, 0, vx, vy, window_flags, NULL);
	// gui_window_t* view_window = gui_create_window("canvas_view", 0, 0, vx,
	// 					      vy, window_flags,
	// 					      gui->windows.elems[0]);
	// vec_push_back(app->windows, &view_window);
	// void* gfx_view_handle = gui_get_window_handle(app->windows.elems[1]);
	const struct gfx_config gfx_cfg = {
		.module = GFX_MODULE_DX11,
		.window = app->window,
		.adapter = GFX_ADAPTER_INDEX,
		.width = vx,
		.height = vy,
		.fps_num = FPS_NUM,
		.fps_den = 1,
		.fullscreen = false,
		.pix_fmt = PIX_FMT_RGBA32,
	};
	gui_set_mouse_mode(MOUSE_MODE_RELATIVE);
	gui_capture_mouse(app->window, true);
	gui_show_mouse(false);
	app->entities = ent_init(MAX_ENTITIES);
	ENSURE_OK(app_init_gfx(app, &gfx_cfg));
	ENSURE_OK(app_init_inputs(app));
	ENSURE_OK(app_init_assets(app, assets_toml_path));
	ENSURE_OK(app_init_meshes(app));
	ENSURE_OK(app_init_scenes(app));

	app->app_time_start = os_get_time_sec();
	app->frame_time_last = app->app_time_start;
	app->game_state = GAME_STATE_INIT;

	// Read .obj mesh
	// {
	// 	buffer_t* file_buf = buffer_new(0);
	// 	const char* file_path = "assets/mesh/gems2_gem4.obj";
	// 	size_t bytes_read = file_read(file_path, file_buf);

	// 	size_t char_count = 0;
	// 	size_t line_count = 0;
	// 	size_t str_len = 0;
	// 	size_t max_lines = 32;
	// 	VECTOR(string_t) lines;
	// 	vec_init(lines);
	// 	char ch = '\0';
	// 	char line[256];
	// 	while (ch != EOF) {
	// 		ch = file_buf->data[char_count];
	// 		line[str_len] = ch;
	// 		char_count++;
	// 		str_len++;
	// 		if (ch == '\n') {
	// 			string_t s;
	// 			str_init(&s);
	// 			str_copy_cstr(&s, &line[0], str_len);
	// 			vec_push_back(lines, &s);
	// 			str_len = 0;
	// 			line_count++;
	// 		}
	// 	}

	// 	u32 num_verts = 0;
	// 	VECTOR(string_t) vert_lines;
	// 	vec_init(vert_lines);
	// 	for (int i = 0; i < lines.num_elems; i++) {
	// 		string_t* s = &lines.elems[i];
	// 		const char* data = str_get(s);
	// 		if (str_starts_with(s, "v ")) {
	// 			num_verts++;
	// 			string_t new_str;
	// 			str_init(&new_str);
	// 			str_copy(&new_str, s);
	// 			vec_push_back(vert_lines, &new_str);
	// 		}
	// 		str_clear(&lines.elems[i]);
	// 	}
	// 	for (int i = 0; i < num_verts; i++) {
	// 		string_t* s = &vert_lines.elems[i];
	// 		printf("%s\n", str_get(s));
	// 		str_clear(&vert_lines.elems[i]);
	// 	}
	// 	gfx_mesh_t* mesh = gfx_mesh_new(GFX_VERTEX_POS_COL, num_verts);

	// 	vec_free(lines);
	// 	vec_free(vert_lines);

	// 	buffer_free(file_buf);
	// 	file_buf = NULL;
	// }

	return RESULT_OK;
}

result app_init_gfx(struct application* app, const struct gfx_config* cfg)
{
	// geom_init();

	ENSURE_OK(gfx_init(cfg, GFX_USE_ZBUFFER));

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

	rect_t viewport = {
		.x = 0,
		.y = 0,
		.w = app->window->bounds.w,
		.h = app->window->bounds.h
	};
	app->cam.cam = gfx_camera_new();
	camera_t perpective_cam = gfx_camera_persp(viewport);
	memcpy(app->cam.cam, &perpective_cam, sizeof(camera_t));

	gfx_init_cimgui();

	return RESULT_OK;
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
	inp_bind_virtual_key(app->inputs, kCommandToggleMouse, SCANCODE_F9);
	inp_bind_virtual_key(app->inputs, kCommandToggleFlyCam, SCANCODE_F8);
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

result app_init_meshes(struct application* app)
{
	vec_init(app->meshes);
	app->mesh_map = hashmap_new();

	vec3f_t cube_sz = { 1.0f, 1.0f, 1.0f };
	gfx_mesh_t *cube_pos_col = gfx_mesh_new(GFX_VERTEX_POS_COL, GFX_CUBE_NUM_VERTS, GFX_CUBE_NUM_INDICES);
	gfx_mesh_t *cube_pos_uvs = gfx_mesh_new(GFX_VERTEX_POS_UV, GFX_CUBE_NUM_VERTS, GFX_CUBE_NUM_INDICES);
	gfx_compute_cube(cube_sz, cube_pos_col->positions, cube_pos_col->indices, true);
	memcpy(cube_pos_uvs->positions, cube_pos_col->positions, sizeof(vec3f_t) * GFX_CUBE_NUM_VERTS);
	memcpy(cube_pos_uvs->indices, cube_pos_col->indices, sizeof(u32) * GFX_CUBE_NUM_INDICES);
	// gfx_reverse_winding(cube_pos_col->indices, GFX_CUBE_NUM_INDICES);

	vec4f_t colors[GFX_CUBE_NUM_VERTS];
	for (size_t c = 0; c < GFX_CUBE_NUM_VERTS; c++) {
		colors[c].x = (f32)random64(0.1, 1.0);
		colors[c].y = (f32)random64(0.1, 1.0);
		colors[c].z = (f32)random64(0.1, 1.0);
		colors[c].w = 1.0f;
	}
	memcpy(cube_pos_col->colors, &colors[0], sizeof(vec4f_t) * GFX_CUBE_NUM_VERTS);
	
	vec2f_t* tv_data = gfx_cube_uvs();
	for (size_t i = 0; i < GFX_CUBE_NUM_VERTS; i++) {
		memcpy((vec2f_t*)cube_pos_uvs->tex_verts[i].data, &tv_data[i], sizeof(vec2f_t));
		cube_pos_uvs->tex_verts[i].size = sizeof(vec2f_t);
	}

	vec_push_back(app->meshes, &cube_pos_col);
	vec_push_back(app->meshes, &cube_pos_uvs);

	hash_key_t cube_key_pos_col = {
		.data = "cube_pos_col",
		.size = 12,
		.seed = kDefaultHashmapSeed
	};
	hashmap_insert(app->mesh_map, cube_key_pos_col, cube_pos_col, sizeof(gfx_mesh_t));

	hash_key_t cube_key_pos_uvs = {
		.data = "cube_pos_uvs",
		.size = 12,
		.seed = kDefaultHashmapSeed
	};
	hashmap_insert(app->mesh_map, cube_key_pos_uvs, cube_pos_uvs, sizeof(gfx_mesh_t));

	return RESULT_OK;
}

void app_free_meshes(struct application* app)
{
	for (u32 i = 0; i < app->meshes.num_elems; i++) {
		gfx_mesh_t* m = *(gfx_mesh_t**)vec_elem(app->meshes, i);
		if (m != NULL) {
			gfx_mesh_free(m);
			m = NULL;
		}
	}
	vec_free(app->meshes);

	hashmap_free(app->mesh_map);
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
	gfx_shader_var_t texture_var = {.name = "texture",
					.type = GFX_SHADER_VAR_TEX,
					.data = NULL,
					.own_data = false};
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

	gfx_mesh_t *cube_mesh = NULL;
	hash_key_t cube_mesh_key = {
		.data = "cube_pos_col",
		.size = 12,
		.seed = kDefaultHashmapSeed
	};
	hashmap_find(app->mesh_map, cube_mesh_key, &cube_mesh);
	// reverse_indices_winding(&cube_ind[0], GFX_CUBE_NUM_INDICES);

	u32 num_rows = 32;
	u32 num_cols = 32;

	// Instanced Scene
	{
		u32 num_instances = num_rows * num_cols;

		asset_t* vs_asset_inst;
		asset_t* ps_asset_inst;
		ENSURE_OK(asset_manager_find("vs_pos_color_instanced", app->assets, &vs_asset_inst));
		ENSURE_OK(asset_manager_find("ps_pos_color_instanced", app->assets, &ps_asset_inst));

		gfx_scene_inst_t* si = gfx_scene_inst_new("box_inst", num_instances);

		gfx_shader_t* vs = (gfx_shader_t*)vs_asset_inst->data;
		gfx_shader_t* ps = (gfx_shader_t*)ps_asset_inst->data;
		si->scene->curr_vertex_shader = vs;
		si->scene->curr_pixel_shader = ps;
		gfx_shader_add_var(vs, view_proj_var);
		size_t cbuf_size = gfx_shader_get_vars_size(vs);
		gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
				GFX_BUFFER_USAGE_DYNAMIC, &vs->cbuffer);
		cbuf_size = gfx_shader_get_vars_size(ps);
		gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
				GFX_BUFFER_USAGE_DYNAMIC, &ps->cbuffer);

		gfx_scene_add_asset(si->scene, vs_asset_inst);
		gfx_scene_add_asset(si->scene, ps_asset_inst);
		gfx_scene_set_mesh(si->scene, cube_mesh);

		float box_scale = 0.09f;
		si->scene->rot_angle = 0.0f;
		si->scene->rot_axis = vec3_set(0.0f, 0.0f, 0.0f);
		si->scene->scale = vec3_set(box_scale, box_scale, box_scale);
		for (u32 row = 0; row < num_rows; row++) {
			for (u32 col = 0; col < num_cols; col++) {
				vec3f_t pos;
				pos.x = ((float)(col)/4.0f)-xy_scale.x*2;
				pos.y = 5.0f + cosf(360.0f - ((float)row + 16.0f * (float)col)* (M_PI/180.0f));
				pos.z = ((float)(row)/4.0f);

				mat4f_t trans_mat = mat4_translate(pos);
				mat4f_t scale_mat = mat4_scale(si->scene->scale);
				mat4f_t rot_mat = mat4_rotate(si->scene->rot_angle, si->scene->rot_axis);
				mat4f_t world_mat = mat4_mul(mat4_mul(mat4_mul(mat4_identity(), rot_mat), trans_mat), scale_mat);

				size_t tdx = row * num_rows + col;
				si->transforms[tdx] = mat4_transpose(world_mat);
			}
		}

		size_t vbuf_size = gfx_mesh_get_size(si->scene->mesh);
		gfx_buffer_new(NULL, vbuf_size, GFX_BUFFER_VERTEX,
			GFX_BUFFER_USAGE_DYNAMIC, &si->vbuf);
		size_t index_buffer_size = sizeof(u32) * BM_GFX_MAX_INDICES;
		gfx_buffer_new(NULL, index_buffer_size, GFX_BUFFER_INDEX,
			GFX_BUFFER_USAGE_DYNAMIC, &si->ibuf);
		gfx_buffer_new(NULL, sizeof(mat4f_t) * num_instances, GFX_BUFFER_VERTEX,
			GFX_BUFFER_USAGE_DYNAMIC, &si->vbuf_xform);

		gfx_scene_inst_copy_buffers(si);

		vec_push_back(app->scene_insts, &si);
	}

	{
		asset_t* tex_asset = NULL;
		ENSURE_OK(asset_manager_find("tiled_wall_64x64", app->assets, &tex_asset));
		ENSURE_OK(asset_manager_find("vs_pos_uv", app->assets, &vs_asset));
		ENSURE_OK(asset_manager_find("ps_pos_uv", app->assets, &ps_asset));

		gfx_shader_t* vs = (gfx_shader_t*)vs_asset->data;
		gfx_shader_t* ps = (gfx_shader_t*)ps_asset->data;

		gfx_scene_t* sc = gfx_scene_new("floor_box");
		gfx_scene_add_asset(sc, vs_asset);
		gfx_scene_add_asset(sc, ps_asset);
		gfx_scene_add_asset(sc, tex_asset);
		gfx_shader_add_var(vs, world_var);
		gfx_shader_add_var(vs, view_proj_var);
		gfx_shader_add_var(ps, texture_var);
		gfx_shader_add_var(ps, viewport_res_var);

		cbuf_size = gfx_shader_get_vars_size(ps);
		gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
				GFX_BUFFER_USAGE_DYNAMIC, &ps->cbuffer);

		cbuf_size = gfx_shader_get_vars_size(vs);
		gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
				GFX_BUFFER_USAGE_DYNAMIC, &vs->cbuffer);

		gfx_mesh_t *cube_mesh = NULL;
		hash_key_t cube_mesh_key = {
			.data = "cube_pos_uvs",
			.size = 12,
			.seed = kDefaultHashmapSeed
		};
		hashmap_find(app->mesh_map, cube_mesh_key, &cube_mesh);

		gfx_scene_set_mesh(sc, cube_mesh);
		gfx_scene_set_pos(sc, vec3_set(0,0,0));
		gfx_scene_set_rotation(sc, 0.0f, vec3_set(0, 0, 0));
		// gfx_scene_set_rotation(sc, 45.0f, vec3_set(0.0f, 1.0f, 0.0f));
		gfx_scene_set_scale(sc, vec3_set(10.0f, 0.1f, 10.0f));
		vec_push_back(app->scenes, &sc);
	}

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

void app_init_game(struct application* app)
{
	entity_t* player = ent_spawn(app->entities, "player", kCameraOrigin,
		ENT_PLAYER|ENT_MOVER|ENT_SHOOTER|ENT_COLLIDER, FOREVER);

	app->cam.sensitivity = kDefaultMouseSensitivity;
	app->cam.fly_cam = false;
	app->cam.yaw = 0.0f;
	app->cam.pitch = 0.0f; //-90.0f;
	app->cam.pitch_min = -90.0; //app->cam.pitch - 90.0f;
	app->cam.pitch_max = 90.0f; //90.0f;
	gfx_camera_set_pos(app->cam.cam, vec3_copy(player->pos));
	gfx_camera_orient(app->cam.cam, app->cam.yaw, app->cam.pitch);

	app->game_state = GAME_STATE_RUN;
}

void app_refresh(struct application* app)
{
	while (app->game_state != GAME_STATE_EXIT) {
		switch (app->game_state) {
		case GAME_STATE_INIT:
			app_init_game(app);
			break;
		case GAME_STATE_RUN:
			app_frame_step(app);
			break;
		case GAME_STATE_EXIT:
			break;
		}
	}
}

void app_refresh_mouse(struct application* app)
{
	static f32 pitch_prev = 0.0f;
	static f32 yaw_prev = 0.0f;
	static bool fly_cam = false;
	if (app->cam.fly_cam != fly_cam) {
		fly_cam = app->cam.fly_cam;
		pitch_prev = 0.0f;
	}
	pitch_prev = app->cam.pitch;
	yaw_prev = app->cam.yaw;

	mouse_t mouse = { 0 };
	gui_read_mouse_state(&mouse);
	inp_update_mouse(app->inputs, &mouse);

	vec2f_t mouse_relative = vec2_mulf(vec2_set((f32)mouse.relative.x, (f32)mouse.relative.y), app->cam.sensitivity);

	app->cam.pitch = clampf(pitch_prev + mouse_relative.y, app->cam.pitch_min, app->cam.pitch_max);

	if (yaw_prev + mouse_relative.x > 360.0f)
		yaw_prev = 0.0f;
	else if (yaw_prev + mouse_relative.x < 0.0f)
		yaw_prev = 360.0f;
	app->cam.yaw = clampf(yaw_prev + mouse_relative.x, 0.0f, 360.0f);

	gfx_camera_orient(app->cam.cam,
		yaw_prev - app->cam.yaw,
		pitch_prev - app->cam.pitch);
}

void app_toggle_mouse_capture(struct application* app)
{
	static bool last_toggle = false;
	static bool toggle = false;
	inp_cmd_toggle(app->inputs, kCommandToggleMouse, &toggle);
	if (toggle != last_toggle) {
		last_toggle = toggle;
		vec2i_t centerpoint = { 0 };
		gui_get_window_centerpoint(app->window, &centerpoint, true);
		gui_set_mouse_mode(toggle ? MOUSE_MODE_NORMAL : MOUSE_MODE_RELATIVE);
		gui_capture_mouse(app->window, toggle);
		if (toggle) {
			gui_move_mouse(centerpoint.x, centerpoint.y);
			gui_show_mouse(toggle);
		}
	}
}

void app_toggle_fly_camera(struct application* app)
{
	static bool last_toggle = false;
	static bool toggle = false;
	inp_cmd_toggle(app->inputs, kCommandToggleFlyCam, &toggle);
	if (toggle != last_toggle) {
		last_toggle = toggle;
		app->cam.fly_cam = !app->cam.fly_cam;
		if (app->cam.fly_cam) {
			app->cam.pitch = 0.0f;
			app->cam.pitch_min = -90;
			app->cam.pitch_max = 90;
		} else {
			app->cam.pitch = -90.0f;
			app->cam.pitch_min = app->cam.pitch - 90.0f;
			app->cam.pitch_max = 90.0f;
		}
	}
}

void app_refresh_entities(struct application* app)
{
	ent_refresh(app->entities, kFriction, app->app_time_start, app->time_now, app->time_delta);
	entity_t* player = ent_by_index(app->entities, ENT_INDEX_PLAYER);
	if (player != NULL) {
		gfx_camera_set_pos(app->cam.cam, player->pos);
	}
}

void app_refresh_gfx(struct application* app)
{
	if (app->frame_timer == 0.0)
		app->frame_timer = os_get_time_sec();

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

	mat4f_t viewproj_mat = mat4_transpose(gfx_camera_get_view_proj(app->cam.cam));
	static f32 y_angle = 0.0f;
	static bool reverse_y_angle = false;
	// static const char* bg_want_sprite = "metro";
	size_t vb_data_size = 0;
	size_t vb_data_offs = 0;
	size_t tex_vert_size = 0;
	size_t vert_stride = 0;

	gfx_cimgui_begin();
	gfx_cimgui_frame(app);

	gfx_render_clear(&kClearColor);
	gfx_system_bind_render_target();
	gfx_toggle_depth(true);
	gfx_bind_primitive_topology(GFX_TOPOLOGY_TRIANGLE_LIST);
	gfx_bind_rasterizer();
	gfx_bind_blend_state();

	for (size_t sdx = 0; sdx < app->scenes.num_elems; sdx++) {
		vb_data_offs = 0;
		gfx_scene_t* scene = (gfx_scene_t*)app->scenes.elems[sdx];
		if (!scene) {
			logger(LOG_WARNING, "app_refresh_gfx: gfx_scene is NULL!");
			continue;
		}

		gfx_shader_t* vs = scene->curr_vertex_shader;
		gfx_shader_t* ps = scene->curr_pixel_shader;
		if (vs != NULL) {
			mat4f_t trans_mat = mat4_translate(scene->pos);
			mat4f_t scale_mat = mat4_scale(scene->scale);
			mat4f_t rot_mat = mat4_rotate(scene->rot_angle, scene->rot_axis);
			mat4f_t world_mat = mat4_transpose(mat4_mul(mat4_mul(mat4_mul(mat4_identity(), rot_mat), trans_mat), scale_mat));
			gfx_shader_set_var_by_name(vs, "world", &world_mat, false);
			gfx_shader_set_var_by_name(vs, "view_proj", &viewproj_mat, false);
		}
		if (ps != NULL) {
			vec2f_t viewport_res = { (f32)app->window->bounds.w, (f32)app->window->bounds.h};
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
		// FIXME: Move this out of the main loop and into gfx/scene init code
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

		for (u32 idx = 0; idx < scene->mesh->num_indices; idx++) {
			memcpy(&ibuf_data[idx], &scene->mesh->indices[idx],
				   sizeof(u32));
		}

		gfx_set_vertex_shader(vs);
		gfx_set_pixel_shader(ps);
		gfx_system_bind_input_layout(vs);

		//TODO: Move sampler state into pixel shader, add state param to bind function
		gfx_shader_var_t* tex_var =
			gfx_shader_get_var_by_name(ps, "texture");
		if (tex_var) {
			gfx_bind_sampler_state((gfx_texture_t*)tex_var->data, 0);
		}

		gfx_bind_vertex_buffer(app->vbuf, 0, (u32)vert_stride, 0);
		gfx_bind_index_buffer(app->ibuf, 0);

		if (gfx_shader_cbuffer_fill(vs) > 0) {
			gfx_buffer_upload_constants(vs);
		}
		if (gfx_shader_cbuffer_fill(ps) > 0) {
			gfx_buffer_upload_constants(ps);
		}

		gfx_buffer_copy(app->vbuf, vbuf_data, vb_data_size);
		gfx_buffer_copy(app->ibuf, ibuf_data,
				scene->mesh->num_indices * sizeof(u32));

		gfx_render_begin(0, BM_GFX_MAX_INDICES, 1, true, false);
	}

	// Render instanced scenes
	for (size_t i = 0; i < app->scene_insts.num_elems; i++) {
		gfx_scene_inst_t* si = app->scene_insts.elems[i];
		if (si == NULL)
			continue;
		gfx_shader_t* vs = si->scene->curr_vertex_shader;
		gfx_shader_t* ps = si->scene->curr_pixel_shader;
		gfx_set_vertex_shader(vs);
		gfx_set_pixel_shader(ps);
		gfx_shader_set_var_by_name(vs, "view_proj", &viewproj_mat, false);

		float box_scale = 0.125f;
		si->scene->rot_angle = 0.0f;
		si->scene->rot_axis = vec3_set(0.0f, 0.0f, 0.0f);
		si->scene->scale = vec3_set(box_scale, box_scale, box_scale);
		u32 num_rows = 32;
		u32 num_cols = 32;
		vec2f_t xy_scale = vec2_divf(vec2_set(1.0f, 0.5625f), 4.0f);
		for (u32 row = 0; row < num_rows; row++) {
			for (u32 col = 0; col < num_cols; col++) {
				vec3f_t pos;
				f32 frow = (f32)row+1;
				f32 fcol = (f32)col+1;
				pos.x = (fcol / 4.0f) - xy_scale.x * 2.0f;
				pos.y = 3.0f + cosf((y_angle * (f32)app->fps_frame_time) - (frow + 4.0f * fcol) * (M_PI / 180.0f));
				pos.z = frow / 4.0f;

				mat4f_t trans_mat = mat4_translate(pos);
				mat4f_t scale_mat = mat4_scale(si->scene->scale);
				mat4f_t rot_mat = mat4_rotate(si->scene->rot_angle, si->scene->rot_axis);
				mat4f_t world_mat = mat4_mul(mat4_mul(mat4_mul(mat4_identity(), rot_mat), trans_mat), scale_mat);

				size_t tdx = row * num_rows + col;
				si->transforms[tdx] = mat4_transpose(world_mat);
			}
		}

		gfx_system_bind_input_layout(vs);

		u32 stride = gfx_get_vertex_stride(si->scene->mesh->type);
		gfx_bind_vertex_buffer(si->vbuf, 0, stride, 0);
		gfx_bind_index_buffer(si->ibuf, 0);
		gfx_render_begin(0, BM_GFX_MAX_INDICES, 1, true, false);

		gfx_buffer_copy(si->vbuf_xform, (const void*)si->transforms, sizeof(mat4f_t) * si->num_instances);
		gfx_bind_vertex_buffer(si->vbuf_xform, 1, sizeof(mat4f_t), 0);
		if (gfx_shader_cbuffer_fill(vs) > 0) {
			gfx_buffer_upload_constants(vs);
		}
		if (gfx_shader_cbuffer_fill(ps) > 0) {
			gfx_buffer_upload_constants(ps);
		}
		gfx_render_begin(0, BM_GFX_MAX_INDICES, si->num_instances, true, true);

		if (reverse_y_angle)
			y_angle -= 1.0f;
		else
			y_angle += 1.0f;
		if (y_angle >= 360.0f || y_angle <= 0.0f) {
			reverse_y_angle = !reverse_y_angle;
		}
		// printf("%.2f %d\n", y_angle, reverse_y_angle);
	}

	gfx_cimgui_end();

	gfx_render_end(false, 0);

	// os_sleep_ms(16);

	app->frame_count++;
}

void app_shutdown(struct application* app)
{
	if (app != NULL) {
		logger(LOG_INFO, "Rendered %lld frames\n", app->frame_count);
		ent_shutdown(app->entities);
		gfx_camera_free(app->cam.cam);

		gfx_buffer_free(app->ibuf);
		gfx_buffer_free(app->vbuf);

		app_free_meshes(app);

		// free scenes
		for (size_t i = 0; i < app->scenes.num_elems; i++) {
			gfx_scene_free(app->scenes.elems[i]);
		}
		vec_free(app->scenes);

		// free instanced scenes
		for (size_t i = 0; i < app->scene_insts.num_elems; i++) {
			gfx_scene_inst_free(app->scene_insts.elems[i]);
		}
		vec_free(app->scene_insts);

		asset_manager_free(app->assets);
		inp_free(app->inputs);
		// geom_free();
		gfx_shutdown();
		gui_shutdown();
	}
}

void set_player_vec(struct application* app, entity_t* player)
{
	vec3f_t forward_vec = gfx_camera_forward(app->cam.cam); // app->cam.fly_cam ? gfx_camera_forward(app->cam.cam) : gfx_camera_up(app->cam.cam);
	vec3f_t backward_vec = gfx_camera_backward(app->cam.cam); // app->cam.fly_cam ? gfx_camera_backward(app->cam.cam) : gfx_camera_down(app->cam.cam);
	if (inp_cmd_get_state(app->inputs, kCommandMoveForward)) { /* W */
		player->acc = vec3_mulf(vec3_add(player->acc, forward_vec), kPlayerSpeed);
	}
	if (inp_cmd_get_state(app->inputs, kCommandMoveBack)) { /* S */
		player->acc = vec3_mulf(vec3_add(player->acc, backward_vec), kPlayerSpeed);
	}
	if (inp_cmd_get_state(app->inputs, kCommandMoveLeft)) { /* A */
		player->acc = vec3_mulf(vec3_add(player->acc, gfx_camera_left(app->cam.cam)), kPlayerSpeed);
	}
	if (inp_cmd_get_state(app->inputs, kCommandMoveRight)) { /* D */
		player->acc = vec3_mulf(vec3_add(player->acc, gfx_camera_right(app->cam.cam)), kPlayerSpeed);
	}
}

void app_frame_step(struct application* app)
{
	f64 now = os_get_time_sec();
	f64 frame_time_remain = now - app->frame_time_last;
	app->frame_time_last = now;

	gui_refresh();

	app_refresh_mouse(app);

	gui_event_t event;
	while (gui_event_pop(&event)) {
		if (event.type == GUI_EVENT_EXIT) {
			app->game_state = GAME_STATE_EXIT;
		} else if (event.type == GUI_EVENT_WINDOW_SIZE) {
			gfx_resize_swap_chain(event.window_width, event.window_height, PIX_FMT_RGBA32);
		} else {
			inp_read_gui_event(app->inputs, &event);
		}
	}

	entity_t* player = ent_by_index(app->entities, ENT_INDEX_PLAYER);
	set_player_vec(app, player);
	if (inp_cmd_get_state(app->inputs, kCommandPlayerPrimaryFire)) { /* LMB (Mouse 1) */
		ent_spawn(app->entities, "bullet", player->pos, ENT_MOVER|ENT_SHOOTER|ENT_COLLIDER, FOREVER);
	}

	if (inp_cmd_get_state(app->inputs, kCommandQuit))
		app->game_state = GAME_STATE_EXIT;

	app_toggle_mouse_capture(app);
	app_toggle_fly_camera(app);

	while (frame_time_remain > 0.0) {
		f64 dt = min(frame_time_remain, app->fps_frame_time);
		app->time_delta = dt;
		app_refresh_entities(app);
		frame_time_remain -= dt;
		app->time_now += dt;
	}

	app_refresh_gfx(app);
	app->frame_perf_time = os_get_time_sec() - now;
}
