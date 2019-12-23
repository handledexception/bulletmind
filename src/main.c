/*

bulletmind
(c) 2019 Paul Hindt
v0.1.122219a

*/

#define _CRT_SECURE_NO_WARNINGS 1
#define DEBUG_PRINT

#include "command.h"
#include "entity.h"
//#include "imgfile.h"
#include "font.h"
#include "input.h"
#include "main.h"
#include "system.h"
#include "timing.h"
#include "vector.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <SDL.h>

engine_t *engine = NULL;

int main(int argc, char** argv)
{
    engine = (engine_t *)malloc(sizeof(engine_t));
    if (engine == NULL) { return -1; }

    if (sys_init(engine) == 0) {
        engine->state = ES_PLAY;
    }
    else {
        printf("sys_init failed!\n");
        return -1;
    }

    font_init(engine->renderer, "7px_font.tga");

    double dt = 0.0;
    engine->target_frametime = TARGET_FRAMETIME(60);
    recti32_t scr = { 0, 0, engine->scr_width, engine->scr_height };
    engine->scr_bounds = scr;

    // main loop
    while(engine->state != ES_QUIT) {
        double frame_start = timing_getsec();

        switch(engine->state) {
            case ES_STARTUP:
                break;
            case ES_PLAY:
                SDL_SetRenderDrawColor(engine->renderer, 0x00, 0x00, 0x00, 0xFF);
                SDL_RenderClear(engine->renderer);

#ifdef DEBUG_PRINT
                char timebuf[256];
                _strtime(timebuf);
                font_print(engine->renderer, 25, 10, 1.0, "Engine Time: %f", timing_enginetime());
                font_print(engine->renderer, 25, 20, 1.0, "Frametime: %f", dt);
                font_print(engine->renderer, 25, 30, 1.0, "Frame Count: %d", engine->frame_count);
                font_print(engine->renderer, 25, 40, 1.0, "Time: %s", timebuf);
#endif
                cmd_refresh(engine);
                sys_refresh();
                ent_refresh(engine, dt);
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