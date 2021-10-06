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

// bulletmind
// (c) 2019-2021 Paul Hindt
// v0.03012021-alpha

#define APP_NAME "bulletmind"
#define APP_VER_MAJ 1
#define APP_VER_MIN 0
#define APP_VER_REV 0
#define APP_VER_KIND "dev"

#include "core/buffer.h"
#include "core/mem_arena.h"
#include "core/string.h"
#include "core/time_convert.h"
#include "core/utils.h"

#include "math/vec2.h"

#include "platform/platform.h"

#include "entity.h"
#include "font.h"
#include "sprite.h"
#include "input.h"
#include "engine.h"
#include "resource.h"
#include "render.h"

#include <SDL.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH_HALF WINDOW_WIDTH / 2
#define WINDOW_HEIGHT_HALF WINDOW_HEIGHT / 2

#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480
#define CAMERA_WIDTH_HALF CAMERA_WIDTH / 2
#define CAMERA_HEIGHT_HALF CAMERA_HEIGHT / 2
#define TARGET_FPS 60.0

#define WORLD_TILES_WIDTH 256
#define WORLD_TILES_HEIGHT 256
#define TILE_WIDTH 64
#define TILE_HEIGHT 64

#define CONSOLE_SPEED 10

static u8* world_map = NULL;

void generate_tilemap(u32 width, u32 height)
{
	world_map = (u8*)malloc(width * height);
	for (size_t i = 0; i < width * width; i++) {
		world_map[i] = rand() % 2;
	}
}

sprite_t* world_map_tile_index(engine_t* engine, s32 x, s32 y, s32 w, s32 h,
			       s32 cam_x, s32 cam_y)
{
	s32 index = x + (y * w);
	sprite_t* tile = NULL;
	game_resource_t* resource = NULL;
	switch (world_map[index]) {
	case 0:
		resource = eng_get_resource(engine, "tiled_wall_64x64");
		tile = (sprite_t*)resource->data;
		break;
	case 1:
		resource = eng_get_resource(engine, "machines_floor_64x64");
		tile = (sprite_t*)resource->data;
		break;
		// case 2:
		// 	resource = eng_get_resource(engine, "shadowy_wall_16x16");
		// 	tile = (sprite_t *)resource->data;
		// 	break;
		// case 3:
		// 	resource = eng_get_resource(engine, "grass_dirt_block");
		// 	tile = (sprite_t *)resource->data;
		// 	break;
	}

	tile->surface->clip_rect.x = x * w - cam_x;
	tile->surface->clip_rect.y = y * h - cam_y;

	return tile;
}

void update_tilemap(engine_t* engine, s32 cam_x, s32 cam_y)
{
	for (s32 y = 0; y < WORLD_TILES_HEIGHT; y++) {
		for (s32 x = 0; x < WORLD_TILES_WIDTH; x++) {
			sprite_t* tile =
				world_map_tile_index(engine, x, y, TILE_WIDTH,
						     TILE_HEIGHT, cam_x, cam_y);

			SDL_RenderCopyEx(engine->renderer, tile->texture, NULL,
					 &tile->surface->clip_rect, 0.0, NULL,
					 SDL_FLIP_NONE);
		}
	}
}

void print_debug_info(engine_t* engine, f64 dt)
{
	if (engine) {
		entity_t* player_ent = ent_by_name(engine->ent_list, "player");
		char time_buf[TEMP_STRING_MAX];
#if defined BM_WINDOWS
		_strtime(time_buf);
#elif defined BM_DARWIN
		time_t raw_time;
		struct tm *info;
   		time( &raw_time );

   		info = localtime( &raw_time );

   		strftime(time_buf, TEMP_STRING_MAX, "%x - %I:%M%p", info);
#endif
		font_print(engine, 10, 10, 1.5, "Time: %s", time_buf);
		font_print(engine, 10, 30, 1.5, "Engine Time: %f",
			   eng_get_time_sec());
		font_print(engine, 10, 50, 1.5, "Frame Time: %f", dt);
		font_print(engine, 10, 70, 1.5, "Frame Count: %d",
			   engine->frame_count);
		font_print(engine, 10, 90, 1.5, "Active Ents: %d",
			   gActiveEntities);
		font_print(engine, 10, 110, 1.5, "Mouse X,Y (%d, %d)",
			   engine->inputs->mouse.window_pos.x,
			   engine->inputs->mouse.window_pos.y);
		font_print(engine, 10, 130, 1.5, "Player Origin (%.2f, %.2f)",
			   player_ent->org.x, player_ent->org.y);
		font_print(engine, 10, 150, 1.5, "Player Velocity (%.2f, %.2f)",
			   player_ent->vel.x, player_ent->vel.y);
		font_print(engine, 10, 170, 1.5,
			   "Left Stick (%d, %d) | Right Stick (%d, %d}",
			   engine->inputs->gamepads[0].axes[0].value,
			   engine->inputs->gamepads[0].axes[1].value,
			   engine->inputs->gamepads[0].axes[2].value,
			   engine->inputs->gamepads[0].axes[3].value);
	}
}

int main(int argc, char** argv)
{
	char s[16] = "\"hello, world!\"";
	str_upper_no_copy(s, 0);
	printf("%s\n", s);

	generate_tilemap(WORLD_TILES_WIDTH, WORLD_TILES_HEIGHT);

	// Allocate memory arena - 8MiB
	arena_init(&g_mem_arena, arena_buf, ARENA_TOTAL_BYTES);

	size_t sz_engine = sizeof(engine_t);
	engine = (engine_t*)arena_alloc(&g_mem_arena, sz_engine,
					DEFAULT_ALIGNMENT);
	memset(engine, 0, sizeof(engine_t));
	engine->adapter_index = -1;			// SDL default to first available
	engine->window_bounds.x = -1;		// SDL window position centered
	engine->window_bounds.y = -1;		// ""
	engine->window_bounds.w = WINDOW_WIDTH;
	engine->window_bounds.h = WINDOW_HEIGHT;
	engine->camera_bounds.x = 0;
	engine->camera_bounds.y = 0;
	engine->camera_bounds.w = CAMERA_WIDTH;
	engine->camera_bounds.h = CAMERA_HEIGHT;
	engine->render_scale.x = (f32)WINDOW_WIDTH / (f32)CAMERA_WIDTH;
	engine->render_scale.y = (f32)WINDOW_HEIGHT / (f32)CAMERA_HEIGHT;
	engine->target_fps = TARGET_FPS;
	for (size_t i = 0; i < MAX_SPAWN_TIMERS; i++)
		engine->spawn_timer[i] = 0.0;
#if defined(BM_DEBUG)
	engine->debug = true;
#else
	engine->debug = false;
#endif
	engine->fullscreen = false;
	engine->console = false;

	s32 con_height = engine->camera_bounds.h / 3;
	engine->console_bounds.x = engine->camera_bounds.x;
	engine->console_bounds.y = engine->camera_bounds.y - con_height;
	engine->console_bounds.w = engine->camera_bounds.w;
	engine->console_bounds.h = con_height;

	rect_t con_start = {
		engine->console_bounds.x,
		engine->console_bounds.y,
		engine->console_bounds.w,
		engine->console_bounds.h
	};

	rect_t con_end = {
		engine->camera_bounds.x,
		engine->camera_bounds.y,
		engine->camera_bounds.w,
		con_height
	};

	const u32 app_version =
		pack_version(APP_VER_MAJ, APP_VER_MIN, APP_VER_REV);

	if (!eng_init(APP_NAME, app_version, engine)) {
		printf("Something went wrong!\n");
		return -1;
	}

	// main loop
	f64 dt = 0.0;
	while (engine->mode != kEngineModeShutdown) {
		u64 frame_start_ns = os_get_time_ns();

		switch (engine->mode) {
		case kEngineModeStartup:
			{
				ent_spawn_player_and_satellite(engine->ent_list, engine->camera_bounds.w, engine->camera_bounds.h);
				eng_play_sound(engine, "theme_music",
						DEFAULT_MUSIC_VOLUME);
				engine->mode = kEngineModePlay;
				break;
			}
		case kEngineModePlay:
		case kEngineModeConsole:
			{
				SDL_SetRenderDrawColor(engine->renderer, 0x20, 0x20,
							0x20, 0xFF);
				SDL_RenderClear(engine->renderer);

				// vec2f_t cam = {0.f, 0.f};
				entity_t* player = ent_by_index(engine->ent_list, 0);
				update_tilemap(engine,
					(u32)player->org.x - TILE_WIDTH,
					(u32)player->org.y - TILE_HEIGHT
				);

				if (engine->debug)
					print_debug_info(engine, dt);

				eng_refresh(engine, dt);

				if (engine->mode == kEngineModeConsole) {
					u8 r, g, b, a;
					SDL_GetRenderDrawColor(engine->renderer, &r, &g, &b, &a);
					rgba_t con_color = { 0x3d, 0x3a, 0x36, 0xff };

					if (engine->console) {

						if (engine->console_bounds.y < con_end.y)
							engine->console_bounds.y += CONSOLE_SPEED;
					}
					else {
						if (engine->console_bounds.y > con_start.y)
							engine->console_bounds.y -= CONSOLE_SPEED;
						else {
							engine->mode = kEngineModePlay;
							engine->inputs->mode = kInputModeGame;
						}
					}

					draw_rect_solid(engine->renderer, engine->console_bounds, &con_color);
					SDL_SetRenderDrawColor(engine->renderer, r, g, b, a); // restore color

					font_print(
						engine,
						engine->console_bounds.x + 8,
						engine->console_bounds.y + engine->console_bounds.h - 20,
						1.5, "> hello, world!");
				}

				break;
			}
		default:
		case kEngineModeQuit:
			{
				eng_stop_music(engine);
				engine->mode = kEngineModeShutdown;
				break;
			}
		}

		do {
			dt = nsec_to_sec_f64(os_get_time_ns() - frame_start_ns);
			if (dt > FRAME_TIME(5)) {
				dt = FRAME_TIME(5);
			}
		} while (dt < engine->target_frametime);
		//printf("%f\n", dt);

		SDL_RenderPresent(engine->renderer);
		engine->frame_count++;
	}

	eng_shutdown(engine);

	engine = NULL;
	return 0;
}
