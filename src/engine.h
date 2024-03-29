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

#pragma once

#include "entity.h"
#include "font.h"
#include "sprite.h"

#include "math/types.h"

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Surface SDL_Surface;
typedef struct SDL_Texture SDL_Texture;

typedef struct input_state_s input_state_t;
typedef struct game_resource_s game_resource_t;
typedef struct audio_state_s audio_state_t;

typedef enum {
	kEngineModeStartup,
	kEngineModePlay,
	kEngineModeConsole,
	kEngineModeQuit,
	kEngineModeShutdown
} engine_mode_t;

#define MAX_SPAWN_TIMERS 1024
#define DEFAULT_SFX_VOLUME 12
#define DEFAULT_MUSIC_VOLUME 25

typedef struct engine_s engine_t;
struct engine_s {
	s32 adapter_index;
	char window_title[TEMP_STRING_MAX];
	SDL_Window* window;
	bool fullscreen;
	SDL_Renderer* renderer;
	// SDL_Surface* scr_surface;
	// SDL_Texture* scr_texture;
	rect_t window_rect;
	rect_t cam_rect;
	rect_t cam_inset;
	vec2f_t render_scale;
	f32 target_fps;
	f64 target_frametime;
	u64 frame_count;
	f64 spawn_timer[MAX_SPAWN_TIMERS];
	engine_mode_t mode;
	bool debug;
	bool console;
	rect_t console_bounds;
	entity_t* ent_list;
	game_resource_t** game_resources;
	font_t font;
	input_state_t* inputs;
	audio_state_t* audio;
};

extern engine_t* engine;

bool eng_init(const char* name, s32 version, engine_t* eng);
void eng_refresh(engine_t* eng, f64 dt);
void eng_shutdown(engine_t* eng);

void eng_init_time(void);
u64 eng_get_time_ns(void);
f64 eng_get_time_sec(void);

game_resource_t* eng_get_resource(engine_t* eng, const char* name);

void eng_play_sound(engine_t* eng, const char* name, s32 volume);
void eng_stop_music(engine_t* eng);

void eng_toggle_fullscreen(engine_t* eng, bool fullscreen);
