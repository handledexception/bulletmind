/*

bulletmind
(c) 2019 Paul Hindt
v0.1.122219a

*/

#if defined(BM_WINDOWS)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#endif

#define APP_NAME "bulletmind"
#define APP_VER_MAJ 1
#define APP_VER_MIN 0
#define APP_VER_REV 0

#include "main.h"
#include "buffer.h"
#include "command.h"
#include "entity.h"
#include "font.h"
#include "sprite.h"
#include "input.h"
#include "memarena.h"
#include "engine.h"
#include "performance.h"
#include "utils.h"
#include "vector.h"

#include <SDL.h>

#include <time.h>

static uint8_t arena_buf[ARENA_TOTAL_BYTES];

void print_debug_info(engine_t* engine, double dt) {
    if (engine) {
        entity_t* player_ent = ent_by_name(engine->ent_list, "player");
        char time_buf[TEMP_STRING_MAX];
#ifdef BM_WINDOWS
        _strtime(time_buf);
#endif
#ifdef BM_LINUX
        time_t t = time(NULL);
        struct tm* tm = localtime(&t);
        strftime(time_buf, sizeof(time_buf), "%c", tm);
#endif
        font_print(engine, 10, 10,  1.5, "Time: %s", time_buf);
        font_print(engine, 10, 30,  1.5, "Engine Time: %f", eng_get_time());
        font_print(engine, 10, 50,  1.5, "Frame Time: %f", dt);
        font_print(engine, 10, 70,  1.5, "Frame Count: %d", engine->frame_count);
        font_print(engine, 10, 90,  1.5, "Active Ents: %d", active_ents);
        font_print(engine, 10, 110, 1.5, "Mouse X,Y (%.2f, %.2f)", engine->mouse_pos.x, engine->mouse_pos.y);
        font_print(engine, 10, 130, 1.5, "Player Origin (%.2f, %.2f)", player_ent->org.x, player_ent->org.y);
        font_print(engine, 10, 150, 1.5, "Player Velocity (%.2f, %.2f)", player_ent->vel.x, player_ent->vel.y);
    }
}

int main(int argc, char** argv) {
    // Allocate memory arena - 8MiB
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
#if defined(BM_DEBUG)
    engine->debug = true;
#else
    engine->debug = false;
#endif
    const uint32_t app_version = pack_version(APP_VER_MAJ, APP_VER_MIN, APP_VER_REV);
    if (!eng_init(APP_NAME, app_version, engine)) {
        printf("Something went wrong!\n");
        return -1;
    }

    // main loop
    double dt = 0.0;
    while(engine->state != ES_QUIT) {
        double frame_start = perf_seconds();

        switch(engine->state) {
            case ES_STARTUP:
                ent_spawn_player_and_satellite(engine->ent_list);
                engine->state = ES_PLAY;
                break;
            case ES_PLAY:
                SDL_SetRenderDrawColor(engine->renderer, 0x20, 0x20, 0x20, 0xFF);
                SDL_RenderClear(engine->renderer);

                if (engine->debug)
                    print_debug_info(engine, dt);

                eng_refresh(engine);
                cmd_refresh(engine);
                ent_refresh(engine, dt);
                break;
            case ES_QUIT:
                break;
        }

        do {
            dt = perf_seconds() - frame_start;
            if (dt > TARGET_FRAMETIME(5)) { dt = TARGET_FRAMETIME(5); }
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
