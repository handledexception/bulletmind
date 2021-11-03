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
#include "font.h"
#include "input.h"
#include "resource.h"

#include "core/logger.h"
#include "core/mem_arena.h"
#include "core/time_convert.h"
#include "core/utils.h"
#include "core/video.h"

#include "platform/platform.h"

#include "gfx/camera.h"

#if defined(BM_WINDOWS)
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
		if (!strcmp(eng->game_resources[rdx]->name, name)) {
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
	// const size_t sz_win_title =
	// 	(sizeof(u8) * strlen(ver_str) + strlen(name)) + 2;
	char window_title[TEMP_STRING_MAX];
	sprintf(window_title, "%s v%s", name, ver_str);

	SDL_Init(SDL_FLAGS);

// #if defined(BM_WINDOWS)
#if 0
	gui_init();
	gui_window_t* wnd =
		gui_create_window("bm window", 100, 100, 640, 480, 0, NULL);
	gui_window_t* wnd2 =
		gui_create_window("bm view", 0, 0, 640, 480, 0, wnd);

	SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version); // must call to get valid HWND??
	SDL_GetWindowWMInfo(eng->window, &wm_info);
	// HWND hwnd = wm_info.info.win.window;
	HWND hwnd = gui_get_window_handle(wnd2);
	const struct gfx_config gfx_cfg = {.adapter = 0,
					   // .width = eng->window_rect.w,
					   // .height = eng->window_rect.h,
					   .width = wnd->w,
					   .height = wnd->h,
					   .fps_num = 60,
					   .fps_den = 1,
					   .pix_fmt = GFX_FORMAT_BGRA,
					   .fullscreen = false,
					   .window = {hwnd}};

	camera_t cam_pers;
	rect_t viewport = {.x = 0, .y = 0, .w = wnd->w, .h = wnd->h};
	vec3f_t cam_pos = {-0.777f, 1.566f, -2.5f};
	vec3f_t cam_dir = {0.f, 0.f, 1.f};
	vec3f_t cam_up = {0.f, 1.f, 0.f};
	gfx_camera_persp(&cam_pers, &cam_pos, &cam_dir, &cam_up, &viewport,
			 75.f, Z_NEAR, Z_FAR);

	gfx_system_t* gfx_sys = gfx_system_init(&gfx_cfg, BM_GFX_D3D11);
	gfx_shader_t* hlsl_vs = gfx_compile_shader_from_file(
		"assets/pos_color.vs.hlsl", "VSMain", kDX11VertexShaderTarget,
		GFX_SHADER_VERTEX);
	gfx_shader_t* hlsl_ps = gfx_compile_shader_from_file(
		"assets/pos_color.ps.hlsl", "PSMain", kDX11PixelShaderTarget,
		GFX_SHADER_PIXEL);
	gfx_build_shader(gfx_sys, hlsl_vs);
	gfx_create_shader_input_layout(gfx_sys, hlsl_vs, GFX_VERTEX_POS_COLOR);
	gfx_build_shader(gfx_sys, hlsl_ps);
	gfx_set_vertex_shader(gfx_sys, hlsl_vs);
	gfx_set_pixel_shader(gfx_sys, hlsl_ps);
	
	mat4f_t world_matrix;
	mat4f_identity(&world_matrix);
	// world_matrix.z.z = 0.5f;
	mat4f_t view_proj_matrix;
	mat4f_mul(&view_proj_matrix, &cam_pers.view_matrix, &cam_pers.proj_matrix);
	mat4f_transpose(&view_proj_matrix, &view_proj_matrix);
	size_t num_vs_vars = 2;
	gfx_shader_var_t* vs_vars = malloc(sizeof(gfx_shader_var_t) * num_vs_vars);
	vs_vars[0].name = "world";
	vs_vars[0].type = GFX_SHADER_PARAM_VEC4;
	vs_vars[0].offset = 0;
	vs_vars[0].data = malloc(sizeof(mat4f_t));
	memcpy(&vs_vars[0].data, &world_matrix, sizeof(mat4f_t));
	vs_vars[0].size = sizeof(mat4f_t);
	vs_vars[1].name = "view_proj";
	vs_vars[1].type = GFX_SHADER_PARAM_VEC4;
	vs_vars[1].offset = (sizeof(mat4f_t) + 15) & ~15;
	vs_vars[1].size = sizeof(mat4f_t);
	vs_vars[1].data = malloc(sizeof(mat4f_t));
	memcpy(&vs_vars[1].data, &view_proj_matrix, sizeof(mat4f_t));
	gfx_buffer_t* cbuffer;
	size_t cbuffer_size = ((sizeof(mat4f_t) * num_vs_vars) + 15) & 0xfffffff0;
	gfx_create_buffer(gfx_sys, NULL, cbuffer_size, GFX_BUFFER_CONSTANT, GFX_BUFFER_USAGE_DYNAMIC, &cbuffer);

	struct gfx_vertex_data* tetrahedron = malloc(sizeof(*tetrahedron));
	tetrahedron->num_vertices = 4;
	tetrahedron->positions = malloc(sizeof(vec3f_t) * 4);
	tetrahedron->colors = malloc(sizeof(vec4f_t) * 4);
	tetrahedron->positions[0] = (vec3f_t){1.f, 1.f, 1.f};
	tetrahedron->positions[1] = (vec3f_t){-1.f, -1.f, 1.f};
	tetrahedron->positions[2] = (vec3f_t){-1.f, 1.f, -1.f};
	tetrahedron->positions[3] = (vec3f_t){1.f, -1.f, -1.f};
	tetrahedron->colors[0] = (vec4f_t){1.f, 0.f, 0.f, 1.f};
	tetrahedron->colors[1] = (vec4f_t){0.f, 1.f, 0.f, 1.f};
	tetrahedron->colors[2] = (vec4f_t){0.f, 0.f, 1.f, 1.f};
	tetrahedron->colors[3] = (vec4f_t){1.f, 1.f, 1.f, 1.f};
	size_t vbd_size = (sizeof(vec3f_t) * 4) + (sizeof(vec4f_t) * 4);
	gfx_buffer_t* vertex_buffer = NULL;
	gfx_create_buffer(gfx_sys, (void*)&tetrahedron, vbd_size,
			  GFX_BUFFER_VERTEX, GFX_BUFFER_USAGE_DEFAULT,
			  &vertex_buffer);

	gfx_init_sampler_state(gfx_sys);
	gfx_init_rasterizer(gfx_sys, GFX_CULLING_NONE, GFX_RASTER_WINDING_CCW);
	gfx_bind_rasterizer(gfx_sys);
	gfx_bind_vertex_buffer(gfx_sys, vertex_buffer, vbd_size, 0);
	gfx_bind_input_layout(gfx_sys, hlsl_vs);
	gfx_bind_primitive_topology(gfx_sys, GFX_TOPOLOGY_TRIANGLE_STRIP);

	while  (1) {
		rgba_t clear_color = {
			.r = 0,
			.g = 0,
			.b = 0,
			.a = 255,
		};
		gfx_render_clear(gfx_sys, &clear_color);
		gfx_upload_constant_buffer(gfx_sys, cbuffer, GFX_SHADER_VERTEX);
		gfx_render_begin(gfx_sys);
		gfx_render_end(gfx_sys, false, 0);
		SDL_Delay(100);
		printf("test\n");
	}
	// gfx_system_shutdown(gfx_sys);
#endif

	s32 window_pos_x = eng->window_rect.x;
	s32 window_pos_y = eng->window_rect.y;
	if (window_pos_x == -1)
		window_pos_x = SDL_WINDOWPOS_CENTERED;
	if (window_pos_y == -1)
		window_pos_y = SDL_WINDOWPOS_CENTERED;

	eng->window = SDL_CreateWindow(window_title, window_pos_x, window_pos_y,
				       eng->window_rect.w, eng->window_rect.h,
				       SDL_WINDOW_SHOWN);

	if (eng->window == NULL) {
		logger(LOG_ERROR, "error creating engine window: %s\n",
		       SDL_GetError());
		return false;
	}

	eng->renderer = SDL_CreateRenderer(eng->window, eng->adapter_index,
					   SDL_RENDERER_ACCELERATED);
	if (eng->renderer == NULL) {
		logger(LOG_ERROR, "error creating engine renderer: %s\n",
		       SDL_GetError());
		return false;
	}

	// SDL_RenderSetViewport(eng->renderer, (SDL_Rect*)&eng->scr_bounds);
	SDL_RenderSetLogicalSize(eng->renderer, eng->camera_rect.w,
				 eng->camera_rect.h);

	// SDL_RenderSetIntegerScale(eng->renderer, true);

	// eng->scr_surface = SDL_CreateRGBSurface(
	//     0,
	//     CAMERA_WIDTH,
	//     CAMERA_HEIGHT,
	//     32,
	//     0xFF000000,
	//     0x00FF0000,
	//     0x0000FF00,
	//     0x000000FF
	// );
	// eng->scr_texture = SDL_CreateTexture(
	//     eng->renderer,
	//     SDL_PIXELFORMAT_RGBA8888,
	//     SDL_TEXTUREACCESS_TARGET,
	//     CAMERA_WIDTH,
	//     CAMERA_HEIGHT
	// );

	eng->inputs = (input_state_t*)arena_alloc(
		&g_mem_arena, sizeof(input_state_t), DEFAULT_ALIGNMENT);
	memset(eng->inputs, 0, sizeof(input_state_t));

	eng->audio = (audio_state_t*)arena_alloc(
		&g_mem_arena, sizeof(audio_state_t), DEFAULT_ALIGNMENT);
	memset(eng->audio, 0, sizeof(audio_state_t));

	if (!audio_init(BM_NUM_AUDIO_CHANNELS, BM_AUDIO_SAMPLE_RATE,
			BM_AUDIO_CHUNK_SIZE))
		return false;
	if (!inp_init(eng->inputs))
		return false;
	if (!game_res_init(eng))
		return false;
	// cmd_init();
	if (!ent_init(&eng->ent_list, MAX_ENTITIES))
		return false;
	eng_init_time();

	eng->font.rsrc = eng_get_resource(eng, "font_7px");
	eng->font.sprite = (sprite_t*)eng->font.rsrc->data;

	eng->target_frametime = FRAME_TIME(eng->target_fps);
	eng->mode = kEngineModeStartup;

	f64 init_end_msec = nsec_to_msec_f64(os_get_time_ns() - init_start);
	logger(LOG_INFO, "eng_init OK [%fms]\n", init_end_msec);

	return true;
}

void eng_refresh(engine_t* eng, f64 dt)
{
	inp_refresh_mouse(&eng->inputs->mouse, eng->render_scale.x,
			  eng->render_scale.y);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		inp_refresh_pressed(eng->inputs, &event);
	}

	cmd_refresh(eng);
	ent_refresh(eng, dt);
}

void eng_shutdown(engine_t* eng)
{
	ent_shutdown(eng->ent_list);
	cmd_shutdown();
	inp_shutdown(eng->inputs);
	audio_shutdown();

	// SDL_FreeSurface(eng->scr_surface);
	// SDL_DestroyTexture(eng->scr_texture);
	SDL_DestroyRenderer(eng->renderer);
	SDL_DestroyWindow(eng->window);

	// eng->scr_texture = NULL;
	eng->renderer = NULL;
	eng->window = NULL;

	SDL_Quit();

	logger(LOG_INFO, "eng_shutdown OK\n\nGoodbye!\n");
}

void eng_play_sound(engine_t* eng, const char* name, s32 volume)
{
	game_resource_t* resource = eng_get_resource(engine, name);
	if (resource != NULL) {
		audio_chunk_t* sound_chunk = (audio_chunk_t*)resource->data;
		if (resource->type == kAssetTypeSoundEffect) {
			sound_chunk->volume = volume;
			int play_ok =
				Mix_PlayChannel(-1, (Mix_Chunk*)sound_chunk, 0);
			if (play_ok < 0)
				logger(LOG_ERROR, "Error playing sound: %s",
				       resource->name);
		} else if (resource->type == kAssetTypeMusic) {
			if (eng->audio->music == NULL) {
				eng->audio->music =
					(Mix_Music*)Mix_LoadMUS(resource->path);
			}
			if (!eng->audio->music_playing) {
				Mix_VolumeMusic(volume);
				Mix_FadeInMusic(eng->audio->music, -1, 150);
				eng->audio->music_volume = volume;
				eng->audio->music_playing = true;
			}
		}
	}
}

void eng_stop_music(engine_t* eng)
{
	if (eng->audio && eng->audio->music_playing)
		Mix_HaltMusic();
}

void eng_toggle_fullscreen(engine_t* eng, bool fullscreen)
{
	// bool is_fullscreen = SDL_GetWindowFlags(eng->window) & SDL_WINDOW_FULLSCREEN;
	SDL_SetWindowFullscreen(eng->window,
				fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

	// SDL_ShowCursor(is_fullscreen);
}
