#include "command.h"
#include "font.h"
#include "input.h"
#include "main.h"
#include "system.h"

#include <SDL.h>

#define SDL_FLAGS (SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER)

engine_t* engine = NULL;
game_resource_t** game_resources = NULL;

bool sys_init(engine_t* eng)
{
    double init_start = timing_getsec();
    eng->frame_count = 0;

    SDL_Init(SDL_FLAGS);
    eng->window = SDL_CreateWindow
    (
        "bulletmind",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        eng->wnd_width, eng->wnd_height,
        SDL_WINDOW_SHOWN
    );

    if(eng->window == NULL) {
        printf("sys_window: %s\n", SDL_GetError());
        return false;
    }

    eng->renderer = SDL_CreateRenderer(eng->window, eng->adapter_index, SDL_RENDERER_ACCELERATED);
    if (eng->renderer == NULL) {
        printf("sys_renderer: %s\n", SDL_GetError());
        return false;
    }

    rect_t scr = { 0, 0, eng->scr_width, eng->scr_height };
    eng->scr_bounds = scr;
    // SDL_RenderSetViewport(eng->renderer, (SDL_Rect*)&eng->scr_bounds);
    SDL_RenderSetLogicalSize(eng->renderer, eng->scr_width, eng->scr_height);
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

    inp_init();
    cmd_init();
    ent_init(&eng->ent_list, MAX_ENTITIES);
    timing_init();
    font_init(eng->renderer, "font_7px.tga");

    eng->target_frametime = TARGET_FRAMETIME(eng->target_fps);
    eng->state = ES_STARTUP;

    printf("sys_init OK [%fms]\n", (timing_getsec() - init_start) * 1000.0);

    return true;
}

void sys_refresh(engine_t* eng)
{
    SDL_Event e;

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    eng->mouse_pos.x = (float)mx;
    eng->mouse_pos.y = (float)my;
    eng->mouse_pos.x /= eng->scr_scale_x;
    eng->mouse_pos.y /= eng->scr_scale_y;

    while (SDL_PollEvent(&e)) {
        switch(e.type) {
            case SDL_KEYDOWN:
                inp_set_key_state(e.key.keysym.scancode, KEY_DOWN);
                break;
            case SDL_KEYUP:
                inp_set_key_state(e.key.keysym.scancode, KEY_UP);
                break;
            case SDL_MOUSEBUTTONDOWN:
                //printf("%d\n", SDL_BUTTON(e.button.button));
                inp_set_mouse_state((e.button.button), e.button.state);
                break;
            case SDL_MOUSEBUTTONUP:
                inp_set_mouse_state((e.button.button), e.button.state);
                break;
        }
    }
}

void sys_shutdown(engine_t* eng)
{
    ent_shutdown(eng->ent_list);
    cmd_shutdown();
    inp_shutdown();

    // SDL_FreeSurface(eng->scr_surface);
    // SDL_DestroyTexture(eng->scr_texture);
    SDL_DestroyRenderer(eng->renderer);
    SDL_DestroyWindow(eng->window);

    // eng->scr_texture = NULL;
    eng->renderer = NULL;
    eng->window = NULL;

    SDL_Quit();
    printf("sys_shutdown OK\n\nGoodbye!\n");
}