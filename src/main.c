/*

bulletmind
(c) 2019 Paul Hindt
v0.1.122219a

*/

#define _CRT_SECURE_NO_WARNINGS 1
#define DEBUG_PRINT

#include "main.h"
#include "buffer.h"
#include "command.h"
#include "entity.h"
#include "font.h"
#include "sprite.h"
#include "input.h"
#include "memarena.h"
#include "system.h"
#include "timing.h"
#include "vector.h"

#include <SDL.h>

void print_debug_info(engine_t* engine, double dt) {
    if (engine) {
        entity_t* player_ent = ent_by_name(engine->ent_list, "player");
        char time_buf[TEMP_STRING_MAX];
        _strtime(time_buf);
        font_print(engine, 10, 10,  1.5, "Time: %s", time_buf);
        font_print(engine, 10, 30,  1.5, "Engine Time: %f", timing_enginetime());
        font_print(engine, 10, 50,  1.5, "Frame Time: %f", dt);
        font_print(engine, 10, 70,  1.5, "Frame Count: %d", engine->frame_count);
        font_print(engine, 10, 90,  1.5, "Active Ents: %d", active_ents);
        font_print(engine, 10, 110, 1.5, "Mouse X,Y (%.2f, %.2f)", engine->mouse_pos.x, engine->mouse_pos.y);
        font_print(engine, 10, 130, 1.5, "Player X,Y (%.2f, %.2f)", player_ent->org.x, player_ent->org.y);
    }
}

int main(int argc, char** argv) {
    arena_init(&mem_arena, arena_buf, ARENA_TOTAL_BYTES);

    size_t sz_engine = sizeof(engine_t);
    engine = (engine_t*)arena_alloc(&mem_arena, sz_engine, DEFAULT_ALIGNMENT);
    engine->adapter_index = -1; // SDL default to first available
    engine->wnd_width = WINDOW_WIDTH;
    engine->wnd_height = WINDOW_HEIGHT;
    engine->scr_width = CAMERA_WIDTH;
    engine->scr_height = CAMERA_HEIGHT;
    engine->scr_scale_x = (float)WINDOW_WIDTH / (float)CAMERA_WIDTH;
    engine->scr_scale_y = (float)WINDOW_HEIGHT / (float)CAMERA_HEIGHT;
    engine->target_fps = 60.f;
    engine->debug = true;
    if (!sys_init(engine)) {
        printf("Something went wrong!\n");
        return -1;
    }

    // sprite_t* raw_sprite = NULL;
    // const uint32_t w = 32;
    // const uint32_t h = 32;
    // const size_t pix_size = w * h * 3;
    // uint8_t test_pattern[pix_size];
    // const uint32_t pixel = 0xcc99ff;
    // for (int idx = 0; idx < pix_size; idx+=3) {
    //     test_pattern[idx] = (pixel>>16) & 0xff;
    //     test_pattern[idx+1] = (pixel>>8) & 0xff;
    //     test_pattern[idx+2] = pixel & 0xff;
    // }

    // sprite_create(test_pattern, w, h, 24, w * 3, SDL_PIXELFORMAT_RGB24, &raw_sprite);
    // sprite_create_texture(engine->renderer, raw_sprite);
    // raw_sprite->surface->clip_rect.x = 120;
    // raw_sprite->surface->clip_rect.y = 120;

    // game_resource_t* raw_sprite_res = arena_alloc(&mem_arena, sizeof(game_resource_t), DEFAULT_ALIGNMENT);
    // sprintf(raw_sprite_res->name, "raw sprite");
    // raw_sprite_res->sprite = raw_sprite;
    // engine->game_resources[2] = raw_sprite_res;

    // main loop
    double dt = 0.0;
    while(engine->state != ES_QUIT) {
        double frame_start = timing_getsec();

        switch(engine->state) {
            case ES_STARTUP:
                ent_spawn_player(engine->ent_list);
                engine->state = ES_PLAY;
                break;
            case ES_PLAY:
                SDL_SetRenderDrawColor(engine->renderer, 0x20, 0x20, 0x20, 0xFF);
                SDL_RenderClear(engine->renderer);

                if (engine->debug)
                    print_debug_info(engine, dt);

                sys_refresh(engine);
                cmd_refresh(engine);
                ent_refresh(engine, dt);
                // SDL_RenderCopyEx(engine->renderer, engine->game_resources[2]->sprite->texture, NULL, &engine->game_resources[1]->sprite->surface->clip_rect, 0.f, NULL, SDL_FLIP_NONE); // raw_sprite
                break;
            case ES_QUIT:
                break;
        }

        do {
            dt = timing_getsec() - frame_start;
            if (dt > TARGET_FRAMETIME(5)) { dt = TARGET_FRAMETIME(5); }
        } while (dt < engine->target_frametime);
        //printf("%f\n", dt);

        SDL_RenderPresent(engine->renderer);
        engine->frame_count++;
    }

    sys_shutdown(engine);

    engine = NULL;
    return 0;
}