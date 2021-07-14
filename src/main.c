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

#if defined(BM_WINDOWS)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#endif

#define APP_NAME "bulletmind"
#define APP_VER_MAJ 1
#define APP_VER_MIN 0
#define APP_VER_REV 0
#define APP_VER_KIND "dev"

#include "main.h"
#include "core/buffer.h"
#include "entity.h"
#include "font.h"
#include "sprite.h"
#include "input.h"
#include "core/mem_arena.h"
#include "engine.h"
#include "resource.h"
#include "core/utils.h"
#include "math/vec2.h"

#include "platform/platform.h"
#include "core/time_convert.h"

#include <SDL.h>

#define WORLD_TILES_WIDTH 64
#define WORLD_TILES_HEIGHT 64
#define TILE_WIDTH 16
#define TILE_HEIGHT 16
static u8 *world_map = NULL;

void generate_tilemap()
{
	world_map = (u8 *)malloc(WORLD_TILES_WIDTH * WORLD_TILES_HEIGHT);
	for (size_t i = 0; i < WORLD_TILES_WIDTH * WORLD_TILES_HEIGHT; i++) {
		world_map[i] = rand() % 3;
	}
}

sprite_t *world_map_tile_index(engine_t *engine, i32 x, i32 y, i32 cam_x,
			       i32 cam_y)
{
	i32 index = x + (y * TILE_WIDTH);
	sprite_t *tile = NULL;
	game_resource_t *resource = NULL;
	switch (world_map[index]) {
	case 0:
		resource = eng_get_resource(engine, "tiled_wall_16x16");
		tile = (sprite_t *)resource->data;
		break;
	case 1:
		resource = eng_get_resource(engine, "tiled_wall_16x16");
		tile = (sprite_t *)resource->data;
		break;
	case 2:
		resource = eng_get_resource(engine, "tiled_wall_16x16");
		tile = (sprite_t *)resource->data;
		break;
	}

	tile->surface->clip_rect.x = x * TILE_WIDTH - cam_x;
	tile->surface->clip_rect.y = y * TILE_HEIGHT - cam_y;

	return tile;
}

void update_tilemap(engine_t *engine, i32 cam_x, i32 cam_y)
{
	for (i32 y = 0; y < WORLD_TILES_HEIGHT; y++) {
		for (i32 x = 0; x < WORLD_TILES_WIDTH; x++) {
			sprite_t *tile = world_map_tile_index(engine, x, y,
							      cam_x, cam_y);
		}
	}
}

void print_debug_info(engine_t *engine, f64 dt)
{
	if (engine) {
		entity_t *player_ent = ent_by_name(engine->ent_list, "player");
		char time_buf[TEMP_STRING_MAX];
		_strtime(time_buf);
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

int main(int argc, char **argv)
{
	// Allocate memory arena - 8MiB
	arena_init(&g_mem_arena, arena_buf, ARENA_TOTAL_BYTES);

	size_t sz_engine = sizeof(engine_t);
	engine = (engine_t *)arena_alloc(&g_mem_arena, sz_engine,
					 DEFAULT_ALIGNMENT);
	memset(engine, 0, sizeof(engine_t));  
	engine->adapter_index = -1; // SDL default to first available
	engine->wnd_width = WINDOW_WIDTH;
	engine->wnd_height = WINDOW_HEIGHT;
	engine->scr_width = CAMERA_WIDTH;
	engine->scr_height = CAMERA_HEIGHT;
	engine->scr_scale_x = (f32)WINDOW_WIDTH / (f32)CAMERA_WIDTH;
	engine->scr_scale_y = (f32)WINDOW_HEIGHT / (f32)CAMERA_HEIGHT;
	engine->target_fps = 60.f;
	for (size_t i = 0; i < MAX_SPAWN_TIMERS; i++)
		engine->spawn_timer[i] = 0.0;
#if defined(BM_DEBUG)
	engine->debug = true;
#else
	engine->debug = false;
#endif
	const u32 app_version =
		pack_version(APP_VER_MAJ, APP_VER_MIN, APP_VER_REV);
	if (!eng_init(APP_NAME, app_version, engine)) {
		printf("Something went wrong!\n");
		return -1;
	}

	// main loop
	f64 dt = 0.0;
	while (engine->state != kEngineStateShutdown) {
		u64 frame_start_ns = os_get_time_ns();

		switch (engine->state) {
		case kEngineStateStartup:
			ent_spawn_player_and_satellite(engine->ent_list);
			eng_play_sound(engine, "theme_music", DEFAULT_MUSIC_VOLUME);
			engine->state = kEngineStatePlay;
			break;
		case kEngineStatePlay:
			SDL_SetRenderDrawColor(engine->renderer, 0x20, 0x20,
					       0x20, 0xFF);
			SDL_RenderClear(engine->renderer);

			if (engine->debug)
				print_debug_info(engine, dt);

			eng_refresh(engine, dt);
			break;
		default:
		case kEngineStateQuit:
			eng_stop_music(engine);
			engine->state = kEngineStateShutdown;
			break;
		}

		do {
			dt = nsec_to_sec_f64(os_get_time_ns() - frame_start_ns);
			if (dt > TARGET_FRAMETIME(5)) {
				dt = TARGET_FRAMETIME(5);
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

#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif
