/*
 * Copyright (c) 2021 Paul Hindt
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "audio.h"
#include "command.h"
#include "engine.h"
#include "entity.h"
#include "font.h"
#include "input.h"
#include "resource.h"

#include "core/logger.h"
#include "core/memory.h"
#include "core/time_convert.h"
#include "core/utils.h"
#include "core/video.h"

#include "platform/platform.h"

#include "gfx/camera.h"
#include "gfx/scene.h"

#if defined(_WIN32)
#include "gfx/gfx_d3d11.h"
#include "gui/gui.h"
#include <SDL_syswm.h>
#endif

#include <SDL.h>
#include <SDL_mixer.h>

#define SDL_FLAGS                                                             \
	(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER | \
	 SDL_INIT_GAMECONTROLLER)

engine_t* engine = NULL;

static u64 engine_start_ticks = 0ULL;

void eng_init_time(void)
{
	engine_start_ticks = os_get_time_ns();
}

u64 eng_get_time_ns(void)
{
	return os_get_time_ns() - engine_start_ticks;
}

f64 eng_get_time_sec(void)
{
	return nsec_to_sec_f64(eng_get_time_ns());
}

game_resource_t* eng_get_resource(engine_t* eng, const char* name)
{
	game_resource_t* rsrc = NULL;
	for (size_t rdx = 0; rdx < MAX_GAME_RESOURCES; rdx++) {
		if (eng->game_resources[rdx] &&
		    eng->game_resources[rdx]->name &&
		    !strcmp(eng->game_resources[rdx]->name, name)) {
			rsrc = eng->game_resources[rdx];
			break;
		}
	}
	return rsrc;
}

bool eng_init(const char* name, s32 version, engine_t* eng)
{
	u64 init_start = os_get_time_ns();
	eng->frame_count = 0;

	// build window title
	char ver_str[12];
	version_string(version, ver_str);
	char window_title[TEMP_STRING_MAX];
	sprintf(window_title, "%s v%s", name, ver_str);

	gui_init();
	gui_window_t* main_window =
		gui_create_window("bm window", 100, 100, 640, 480, 0, NULL);
	gui_window_t* view_window =
		gui_create_window("bm view", 0, 0, 640, 480, 0, main_window);
	HWND view_hwnd = gui_get_window_handle(view_window);
	vec_init(eng->windows);
	vec_push_back(eng->windows, &main_window);
	vec_push_back(eng->windows, &view_window);

	const struct gfx_config gfx_cfg = {.adapter = 0,
					   .width = eng->window_rect.w,
					   .height = eng->window_rect.h,
					   //    .width = wnd->w,
					   //    .height = wnd->h,
					   .fps_num = 60,
					   .fps_den = 1,
					   .pix_fmt = GFX_FORMAT_BGRA,
					   .fullscreen = false,
					   .window = {view_hwnd}};

	rect_t viewport = {
		.x = 0, .y = 0, .w = eng->window_rect.w, .h = eng->window_rect.h,
		// .w = wnd->w, .h = wnd->h,
	};
	// vec3f_t cam_pos = {-0.777f, 1.566f, -2.5f};
	vec3f_t cam_eye = {0.f, 0.25f, -1.f};
	vec3f_t cam_dir = {0.f, 0.f, 0.f};
	vec3f_t cam_up = {0.f, 1.f, 0.f};
	gfx_camera_init(&eng->gfx.camera);
	gfx_camera_persp(&eng->gfx.camera, &cam_eye, &cam_dir, &cam_up,
			 &viewport, 60.f, Z_NEAR, Z_FAR);
	logger(LOG_INFO,
	       "\033[7mgfx\033[m Viewport: %dx%d\nCreated perspective camera...\n",
	       viewport.w, viewport.h);

	bool zstencil_enabled = true;
	u32 gfx_sys_flags = GFX_D3D11 |
			    (zstencil_enabled ? GFX_USE_ZBUFFER : 0);
	eng->gfx.system = gfx_system_init(&gfx_cfg, gfx_sys_flags);
	if (eng->gfx.system == NULL) {
		gfx_system_shutdown(eng->gfx.system);
		logger(LOG_INFO,
		       "\033[7mgfx\033[m Error initializing gfx system!");
	}

	if (!game_res_init(eng)) {
		logger(LOG_ERROR, "error initializing game resources!");
		return false;
	}
	if (!ent_init(&eng->ent_list, MAX_ENTITIES)) {
		logger(LOG_ERROR, "error initializing entities!");
		return false;
	}

	if (!inp_init(eng->inputs)) {
		logger(LOG_ERROR, "error initializing inputs!");
		return false;
	}

	game_resource_t* resource = eng_get_resource(eng, "pos_color_vs");
	gfx_vertex_shader_t* hlsl_vs = (gfx_vertex_shader_t*)resource->data;
	resource = eng_get_resource(eng, "pos_color_ps");
	gfx_shader_t* hlsl_ps = (gfx_shader_t*)resource->data;
	gfx_set_vertex_shader(eng->gfx.system, hlsl_vs);
	gfx_set_pixel_shader(eng->gfx.system, hlsl_ps);
	u32 num_shader_vars = 2;
	gfx_shader_var_t world_var;
	world_var.name = "world";
	world_var.type = GFX_SHADER_PARAM_MAT4;
	world_var.data = mem_alloc(sizeof(struct mat4f));
	world_var.offset = 0;

	gfx_shader_var_t view_proj_var;
	view_proj_var.name = "view_proj";
	view_proj_var.type = GFX_SHADER_PARAM_MAT4;
	view_proj_var.data = mem_alloc(sizeof(struct mat4f));
	view_proj_var.offset = sizeof(struct mat4f);

	struct mat4f* world_matrix = (struct mat4f*)world_var.data;
	struct mat4f* view_proj_matrix = (struct mat4f*)view_proj_var.data;
	mat4f_t trans_matrix, scale_matrix;
	mat4f_identity(world_matrix);
	mat4f_identity(view_proj_matrix);
	mat4f_identity(&trans_matrix);
	mat4f_identity(&scale_matrix);
	const vec4f_t trans_vec = {0.f, -0.5f, 1.0f, 1.f};
	const vec4f_t scale_vec = {1.f, 1.f, 1.f, 1.f};
	mat4f_translate(&trans_matrix, &trans_vec);
	mat4f_scale(&scale_matrix, &scale_vec);
	mat4f_mul(world_matrix, &trans_matrix, &scale_matrix);
	mat4f_mul(view_proj_matrix, &eng->gfx.camera.view_matrix,
		  &eng->gfx.camera.proj_matrix);
	mat4f_transpose(view_proj_matrix, view_proj_matrix);

	size_t cbuffer_size = ((sizeof(mat4f_t) * num_shader_vars) + 15) &
			      0xfffffff0;
	eng->gfx.cbuffer_size = cbuffer_size;
	eng->gfx.cbuffer_data = (u8*)mem_alloc(cbuffer_size);
	gfx_buffer_create(eng->gfx.system, eng->gfx.cbuffer_data, cbuffer_size,
			  GFX_BUFFER_CONSTANT, GFX_BUFFER_USAGE_DYNAMIC,
			  &eng->gfx.cbuffer);
	memcpy(&eng->gfx.cbuffer_data[0], (const void*)world_matrix,
	       sizeof(mat4f_t));
	memcpy(&eng->gfx.cbuffer_data[(sizeof(mat4f_t) + 15) & ~15],
	       (const void*)view_proj_matrix, sizeof(mat4f_t));

	vec_init(eng->gfx.scenes);
	struct gfx_scene* scene = gfx_scene_new(6, 2, GFX_VERTEX_POS_COLOR);
	vec_init(scene->shader_vars);
	scene->vert_data->positions[0] = (vec3f_t){-0.5f, 0.f, -0.5f};
	scene->vert_data->positions[1] = (vec3f_t){0.5f, 0.f, -0.5f};
	scene->vert_data->positions[2] = (vec3f_t){0.5f, 0.f, 0.5f};
	scene->vert_data->positions[3] = (vec3f_t){-0.5f, 0.f, -0.5f};
	scene->vert_data->positions[4] = (vec3f_t){0.5f, 0.f, 0.5f};
	scene->vert_data->positions[5] = (vec3f_t){-0.5f, 0.f, 0.5f};
	scene->vert_data->colors[0] = (vec4f_t){1.f, 0.f, 0.f, 1.f};
	scene->vert_data->colors[1] = (vec4f_t){0.f, 1.f, 0.f, 1.f};
	scene->vert_data->colors[2] = (vec4f_t){0.f, 0.f, 1.f, 1.f};
	scene->vert_data->colors[3] = (vec4f_t){1.f, 0.f, 0.f, 1.f};
	scene->vert_data->colors[4] = (vec4f_t){0.f, 1.f, 0.f, 1.f};
	scene->vert_data->colors[5] = (vec4f_t){0.f, 0.f, 1.f, 1.f};

	struct gfx_scene* scene2 = gfx_scene_new(3, 2, GFX_VERTEX_POS_COLOR);
	vec_init(scene2->shader_vars);
	scene2->vert_data->positions[0] = (vec3f_t){-0.5f, 0.f, 0.5f};
	scene2->vert_data->positions[1] = (vec3f_t){0.5f, 0.f, 0.5f};
	scene2->vert_data->positions[2] = (vec3f_t){0.f, 0.5f, 0.5f};
	scene2->vert_data->colors[0] = (vec4f_t){1.f, 0.f, 0.f, 1.f};
	scene2->vert_data->colors[1] = (vec4f_t){0.f, 1.f, 0.f, 1.f};
	scene2->vert_data->colors[2] = (vec4f_t){0.f, 0.f, 1.f, 1.f};
	size_t vbd_size = (sizeof(vec3f_t) * 9) + (sizeof(vec4f_t) * 9);
	eng->gfx.vbuffer_data = (u8*)mem_alloc(vbd_size);
	gfx_shader_add_var(hlsl_vs, &world_var);
	gfx_shader_add_var(hlsl_vs, &view_proj_var);
	vec_push_back(scene->shader_vars, &world_var);
	vec_push_back(scene->shader_vars, &view_proj_var);
	vec_push_back(scene2->shader_vars, &world_var);
	vec_push_back(scene2->shader_vars, &view_proj_var);
	vec_push_back(eng->gfx.scenes, &scene);
	vec_push_back(eng->gfx.scenes, &scene2);

	size_t vertex_buffer_size = (sizeof(vec3f_t) * BM_GFX_MAX_VERTICES) +
				    (sizeof(vec4f_t) * BM_GFX_MAX_VERTICES);
	eng->gfx.vbuffer_data = (u8*)mem_alloc(vertex_buffer_size);
	gfx_buffer_create(eng->gfx.system, eng->gfx.vbuffer_data,
			  vertex_buffer_size, GFX_BUFFER_VERTEX,
			  GFX_BUFFER_USAGE_DYNAMIC, &eng->gfx.vertex_buffer);

	gfx_init_sampler_state(eng->gfx.system);
	gfx_init_rasterizer(eng->gfx.system, GFX_CULLING_NONE, 0);

	eng->target_frametime = FRAME_TIME(eng->target_fps);
	eng->mode = kEngineModeStartup;
	f64 init_end_msec = nsec_to_msec_f64(os_get_time_ns() - init_start);
	logger(LOG_INFO, "eng_init OK [%fms]\n", init_end_msec);

	return true;
}

void eng_refresh_commands(engine_t* eng)
{
	inp_cmd_toggle(eng->inputs, kCommandDebugMode, &eng->debug);

	// toggle fullscreen
	static bool fullscreen = false;
	inp_cmd_toggle(eng->inputs, kCommandToggleFullscreen, &fullscreen);
	if (fullscreen != eng->fullscreen) {
		eng->fullscreen = fullscreen;
		logger(LOG_DEBUG, "Fullscreen toggled: %d", fullscreen);
	}
	eng_toggle_fullscreen(eng, eng->fullscreen);

	// toggle console
	inp_cmd_toggle(eng->inputs, kCommandConsole, &eng->console);
	if (eng->console) {
		eng->mode = kEngineModeConsole;
		eng->inputs->mode = kInputModeConsole;
	}

	if (inp_cmd_get_state(eng->inputs, kCommandQuit))
		eng->mode = kEngineModeQuit;
	if (inp_cmd_get_state(eng->inputs, kCommandSetFpsHigh) == true)
		eng->target_frametime = FRAME_TIME(eng->target_fps);
	if (inp_cmd_get_state(eng->inputs, kCommandSetFpsLow) == true)
		eng->target_frametime = FRAME_TIME(10);
}

void eng_refresh(engine_t* eng, f64 dt)
{
	// inp_refresh_mouse(&eng->inputs->mouse, eng->render_scale.x,
	// 		  eng->render_scale.y);

	gui_refresh();

	gui_event_t evt;
	while (gui_poll_event(&evt)) {
		inp_refresh_pressed(eng->inputs, &evt);
	}

	struct mouse_device mouse;
	gui_get_global_mouse_state(&mouse);

	// logger(LOG_DEBUG, "mouse pos %d, %d butttons %d %d %d %d %d\n",
	// 	mouse.screen_pos.x,
	// 	mouse.screen_pos.y,
	// 	mouse.buttons[GUI_MOUSE_BUTTON_LEFT].state,
	// 	mouse.buttons[GUI_MOUSE_BUTTON_RIGHT].state,
	// 	mouse.buttons[GUI_MOUSE_BUTTON_MIDDLE].state,
	// 	mouse.buttons[GUI_MOUSE_BUTTON_X1].state,
	// 	mouse.buttons[GUI_MOUSE_BUTTON_X2].state);
	// if (evt.keyboard.keys[GUI_SCANCODE_W].state == GUI_KEY_DOWN)
	// if (gui->keyboard.key_states[GUI_SCANCODE_W] == GUI_KEY_DOWN)
	// 	printf("W DOWN\n");

	// printf("mx = %d, my = %d\n", gui->mouse.screen_pos.x, gui->mouse.screen_pos.y);

	eng_refresh_commands(eng);
	ent_refresh(eng, dt);

	// This is our "scene" .. copy its vertex data into the vertex buffer.
	rgba_t clear_color = {
		.r = 0,
		.g = 0,
		.b = 0,
		.a = 255,
	};
	gfx_render_clear(eng->gfx.system, &clear_color);

	size_t vbd_offset = 0;
	size_t vbd_size = 0;
	size_t cbd_offset = 0;
	size_t vertex_stride =
		gfx_get_vertex_stride(gfx_get_vertex_type(eng->gfx.system));
	for (size_t i = 0; i < eng->gfx.scenes.num_elems; i++) {
		struct gfx_scene* scene =
			(struct gfx_scene*)eng->gfx.scenes.elems[i];

		// Copy vertex buffer data
		for (u32 i = 0; i < scene->vert_data->num_vertices; i++) {
			memcpy((void*)&eng->gfx.vbuffer_data[vbd_offset],
			       (const void*)&scene->vert_data->positions[i],
			       sizeof(struct vec3f));
			vbd_offset += sizeof(struct vec3f);
			memcpy((void*)&eng->gfx.vbuffer_data[vbd_offset],
			       (const void*)&scene->vert_data->colors[i],
			       sizeof(struct vec4f));
			vbd_offset += sizeof(struct vec4f);
		}
		vbd_size += (sizeof(vec3f_t) * scene->vert_data->num_vertices) +
			    (sizeof(vec4f_t) * scene->vert_data->num_vertices);

		// Copy constant buffer data (AKA shader vars) into the constant buffer
		for (size_t vdx = 0; vdx < scene->shader_vars.num_elems;
		     vdx++) {
			gfx_shader_var_t* shader_var =
				&scene->shader_vars.elems[vdx];
			size_t shader_var_size =
				gfx_get_shader_var_size(shader_var->type);
			memcpy(&eng->gfx.cbuffer_data[cbd_offset],
			       (const void*)shader_var->data, shader_var_size);
			cbd_offset += (shader_var_size + 15) & ~15;
		}
	}
	gfx_buffer_copy(eng->gfx.system, eng->gfx.vertex_buffer,
			eng->gfx.vbuffer_data, vbd_size);

	gfx_system_bind_render_target(eng->gfx.system);
	gfx_toggle_zstencil(eng->gfx.system, true);
	gfx_bind_primitive_topology(eng->gfx.system,
				    GFX_TOPOLOGY_TRIANGLE_LIST);
	gfx_bind_vertex_shader_input_layout(eng->gfx.system);
	gfx_bind_rasterizer(eng->gfx.system);
	gfx_bind_sampler_state(eng->gfx.system, NULL, 0);

	gfx_bind_vertex_buffer(eng->gfx.system, eng->gfx.vertex_buffer,
			       (u32)vertex_stride, 0);
	gfx_buffer_copy(eng->gfx.system, eng->gfx.cbuffer,
			eng->gfx.cbuffer_data, eng->gfx.cbuffer_size);
	gfx_buffer_upload_constants(eng->gfx.system, eng->gfx.cbuffer,
				    GFX_SHADER_VERTEX);
	gfx_render_begin(eng->gfx.system);
	gfx_render_end(eng->gfx.system, false, 0);
}

void eng_shutdown(engine_t* eng)
{
	gfx_system_shutdown(eng->gfx.system);
	for (size_t i = 0; i < eng->windows.num_elems; i++) {
		vec_erase(eng->windows, i--);
	}
	gui_shutdown();
	logger(LOG_INFO, "eng_shutdown OK\n\nGoodbye!\n");
}

void eng_play_sound(engine_t* eng, const char* name, s32 volume)
{
	game_resource_t* resource = eng_get_resource(engine, name);
	if (resource != NULL) {
		// audio_chunk_t* sound_chunk = (audio_chunk_t*)resource->data;
		// if (resource->type == kAssetTypeSoundEffect) {
		// 	sound_chunk->volume = volume;
		// 	int play_ok =
		// 		Mix_PlayChannel(-1, (Mix_Chunk*)sound_chunk, 0);
		// 	if (play_ok < 0)
		// 		logger(LOG_ERROR,  "Error playing sound: %s",
		// 		       resource->name);
		// } else if (resource->type == kAssetTypeMusic) {
		// 	if (eng->audio->music == NULL) {
		// 		eng->audio->music =
		// 			(Mix_Music*)Mix_LoadMUS(resource->path);
		// 	}
		// 	if (!eng->audio->music_playing) {
		// 		Mix_VolumeMusic(volume);
		// 		Mix_FadeInMusic(eng->audio->music, -1, 150);
		// 		eng->audio->music_volume = volume;
		// 		eng->audio->music_playing = true;
		// 	}
		// }
	}
}

void eng_stop_music(engine_t* eng)
{
	// if (eng->audio && eng->audio->music_playing)
	// 	Mix_HaltMusic();
}

void eng_toggle_fullscreen(engine_t* eng, bool fullscreen)
{
	// bool is_fullscreen = SDL_GetWindowFlags(eng->window) & SDL_WINDOW_FULLSCREEN;
	// SDL_SetWindowFullscreen(eng->window,
	// 			fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

	// SDL_ShowCursor(is_fullscreen);
}
