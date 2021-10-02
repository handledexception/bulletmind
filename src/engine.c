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

#include "gfx/gfx_d3d11.h"
#include "gui/gui.h"

#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_mixer.h>

#define SDL_FLAGS                                            \
	(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER | \
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
	const size_t sz_win_title =
		(sizeof(u8) * strlen(ver_str) + strlen(name)) + 2;
	char window_title[sz_win_title];
	sprintf(window_title, "%s v%s", name, ver_str);

	SDL_Init(SDL_FLAGS);

	s32 window_pos_x = eng->window_bounds.x;
	s32 window_pos_y = eng->window_bounds.y;
	if (window_pos_x == -1)
		window_pos_x = SDL_WINDOWPOS_CENTERED;
	if (window_pos_y == -1)
		window_pos_y = SDL_WINDOWPOS_CENTERED;

	eng->window = SDL_CreateWindow(window_title,
		window_pos_x, window_pos_y,
		eng->window_bounds.w, eng->window_bounds.h,
		SDL_WINDOW_SHOWN);

	if (eng->window == NULL) {
		logger(LOG_ERROR, "error creating engine window: %s\n",
		       SDL_GetError());
		return false;
	}

	// const size_t sz_sixteen_megabytes = 64 * 1000 * 1000;
	// char* src_buf = malloc(sz_sixteen_megabytes);
	// f64 start_ms = os_get_time_msec();
	// memset(src_buf, 1, sz_sixteen_megabytes);
	// printf("%zu %f\n", sz_sixteen_megabytes, os_get_time_msec() - start_ms);
#define BM_TEST_D3D11
#ifdef BM_TEST_D3D11
	gui_init();
	gui_window_t* wnd = gui_create_window("bm window", 100, 100, 640, 480, 0, NULL);
	gui_window_t* wnd2 = gui_create_window("bm view", 0, 0, 640, 480, 0, wnd);

	SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version); // must call to get valid HWND??
	SDL_GetWindowWMInfo(eng->window, &wm_info);
	// HWND hwnd = wm_info.info.win.window;
	HWND hwnd = gui_get_window_handle(wnd2);
	const struct gfx_config gfx_cfg = {
		.adapter = 0,
		// .width = eng->window_bounds.w,
		// .height = eng->window_bounds.h,
		.width = wnd->w,
		.height = wnd->h,
		.fps_num = 60,
		.fps_den = 1,
		.pix_fmt = GFX_FORMAT_BGRA,
		.fullscreen = false,
		.window = { hwnd }
	};
	gfx_system_t* gfx_sys = gfx_system_init(&gfx_cfg, BM_GFX_D3D11);
	gfx_shader_t* hlsl_vs = gfx_compile_shader_from_file(
		"assets/solid.vs.hlsl", "VSMain", kDX11VertexShaderTarget, GFX_SHADER_VERTEX);
	gfx_shader_t* hlsl_ps = gfx_compile_shader_from_file(
		"assets/solid.ps.hlsl", "PSMain", kDX11PixelShaderTarget, GFX_SHADER_PIXEL);
	gfx_build_shader(gfx_sys, hlsl_vs);
	gfx_build_shader(gfx_sys, hlsl_ps);
	rgba_t clear_color = {
		.r = 0.f,
		.g = 0.f,
		.b = 0.f,
		.a = 1.f,
	};
	gfx_render_clear(gfx_sys, &clear_color);
	gfx_render_begin(gfx_sys);
	gfx_render_end(gfx_sys, false, 0);
	// gfx_system_shutdown(gfx_sys);
#endif

	eng->renderer = SDL_CreateRenderer(eng->window, eng->adapter_index,
					   SDL_RENDERER_ACCELERATED);
	if (eng->renderer == NULL) {
		logger(LOG_ERROR, "error creating engine renderer: %s\n",
		       SDL_GetError());
		return false;
	}

	// SDL_RenderSetViewport(eng->renderer, (SDL_Rect*)&eng->scr_bounds);
	SDL_RenderSetLogicalSize(eng->renderer, eng->camera_bounds.w,
				 eng->camera_bounds.h);

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

			Mix_PlayChannel(-1, (Mix_Chunk*)sound_chunk, 0);
		} else if (resource->type == kAssetTypeMusic) {
			if (eng->audio->music == NULL) {
				eng->audio->music =
					(Mix_Music*)Mix_LoadMUS(resource->path);
			}
			if (!eng->audio->music_playing) {
				Mix_VolumeMusic(volume);
				Mix_FadeInMusic(eng->audio->music, -1, 250);
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
    SDL_SetWindowFullscreen(eng->window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
	printf("FULLSCREEN %d", fullscreen);
    // SDL_ShowCursor(is_fullscreen);
}
