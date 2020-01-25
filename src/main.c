/*

bulletmind
(c) 2019 Paul Hindt
v0.1.122219a

*/

#define _CRT_SECURE_NO_WARNINGS 1
#define DEBUG_PRINT

#include "main.h"
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

int main(int argc, char** argv)
{
    arena_init(&mem_arena, arena_buf, ARENA_TOTAL_BYTES);

    size_t sz_engine = sizeof(engine_t);
    engine = (engine_t*)arena_alloc(&mem_arena, sz_engine, DEFAULT_ALIGNMENT);
    engine->adapter_index = -1; // SDL default to first available
    engine->wnd_width = WINDOW_WIDTH;
    engine->wnd_height = WINDOW_HEIGHT;
    engine->scr_width = CAMERA_WIDTH;
    engine->scr_height = CAMERA_HEIGHT;
    engine->scr_scale_x = WINDOW_WIDTH / CAMERA_WIDTH;
    engine->scr_scale_y = WINDOW_HEIGHT / CAMERA_HEIGHT;
    engine->target_fps = 60.f;
    engine->debug = true;
    if (!sys_init(engine)) {
        printf("Something went wrong!\n");
        return -1;
    }

    /*
    TODO: make functions for allocating, initializing, etc. of game resource objects
    - create_game_resource(name, image)
    - get_game_resource(name)
    */
    game_resources = arena_alloc(&mem_arena, sizeof(game_resource_t*) * MAX_GAME_RESOURCES, DEFAULT_ALIGNMENT);
    game_resource_t* sprite_res = arena_alloc(&mem_arena, sizeof(game_resource_t), DEFAULT_ALIGNMENT);
    // game_resource_t* font_res = arena_alloc(&mem_arena, sizeof(game_resource_t), DEFAULT_ALIGNMENT);
    img_file_t* sprite = NULL;
    // img_file_t* font = NULL;
    // img_file_load("font_7px.tga", &font);
    // img_file_create_texture(engine->renderer, font);
    img_file_load("bullet_ps.tga", &sprite);
    img_file_create_texture(engine->renderer, sprite);
    sprintf(sprite_res->name, "bullet");
    sprite_res->img_file = sprite;
    // sprintf(font_res->name, "7px font");
    // font_res->img_file = font;
    game_resources[0] = sprite_res;
    // game_resources[1] = font_res;

    img_file_t* raw_sprite = NULL;
    const uint32_t w = 32;
    const uint32_t h = 32;
    const size_t pix_size = w * h * 3;
    uint8_t test_pattern[pix_size];
    const uint32_t pixel = 0xcc99ff;
    for (int idx = 3; idx < pix_size-4; idx+=3) {
        test_pattern[idx] = (pixel>>16) & 0xff;
        test_pattern[idx+1] = (pixel>>8) & 0xff;
        test_pattern[idx+2] = pixel & 0xff;
    }
    
    img_file_create(test_pattern, w, h, 24, w * 3, SDL_PIXELFORMAT_RGB24, &raw_sprite);
    img_file_create_texture(engine->renderer, raw_sprite);
    raw_sprite->surface->clip_rect.x = 120;
    raw_sprite->surface->clip_rect.y = 120;

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
                SDL_SetRenderDrawColor(engine->renderer, 0x00, 0x00, 0x00, 0xFF);
                SDL_RenderClear(engine->renderer);

                SDL_RenderCopyEx(engine->renderer, game_resources[0]->img_file->texture, NULL, &game_resources[0]->img_file->surface->clip_rect, 0.f, NULL, SDL_FLIP_NONE); // bullet
                SDL_RenderCopyEx(engine->renderer, raw_sprite->texture, NULL, &raw_sprite->surface->clip_rect, 0.f, NULL, SDL_FLIP_NONE); // test_pattern

                if (engine->debug) {
                    char time_buf[TEMP_STRING_MAX];
                    _strtime(time_buf);
                    font_print(engine->renderer, 5, 5,  1.0, "Time: %s", time_buf);
                    font_print(engine->renderer, 5, 14, 1.0, "Engine Time: %f", timing_enginetime());
                    font_print(engine->renderer, 5, 23, 1.0, "Frame Time: %f", dt);
                    font_print(engine->renderer, 5, 32, 1.0, "Frame Count: %d", engine->frame_count);
                    font_print(engine->renderer, 5, 41, 1.0, "Active Ents: %d", active_ents);
                    font_print(engine->renderer, 5, 50, 1.0, "Mouse X,Y (%d, %d)", engine->mouse_pos.x, engine->mouse_pos.y);
                }
                sys_refresh(engine);
                cmd_refresh(engine);
                ent_refresh(engine, game_resources, dt);
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

    font_shutdown();
    sys_shutdown(engine);

    engine = NULL;
    return 0;
}