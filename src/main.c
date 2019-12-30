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
#include "input.h"
#include "system.h"
#include "timing.h"
#include "vector.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <SDL.h>

int main(int argc, char** argv)
{
    size_t sz_engine = sizeof(engine_t);
    engine = (engine_t*)malloc(sz_engine);
    engine->adapter_index = -1; // SDL default to first available
    engine->scr_width = WINDOW_WIDTH;
    engine->scr_height = WINDOW_HEIGHT;
    engine->target_fps = 60.f;
    if (!sys_init(engine)) {
        printf("Something went wrong!\n");
        return -1;
    }

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

#ifdef DEBUG_PRINT
                char time_buf[TEMP_STRING_MAX];
                _strtime(time_buf);
                font_print(engine->renderer, 25, 10, 1.0, "Engine Time: %f", timing_enginetime());
                font_print(engine->renderer, 25, 20, 1.0, "Frame Time: %f", dt);
                font_print(engine->renderer, 25, 30, 1.0, "Frame Count: %d", engine->frame_count);
                font_print(engine->renderer, 25, 40, 1.0, "Time: %s", time_buf);
                font_print(engine->renderer, 25, 50, 1.0, "Active Ents: %d", active_ents);
#endif
                sys_refresh();
                cmd_refresh(engine);
                ent_refresh(engine->ent_list, dt);
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

    free(engine);
    engine = NULL;
    return 0;
}