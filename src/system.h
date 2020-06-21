#ifndef _H_SYSTEM
#define _H_SYSTEM

#include "c99defs.h"
#include "entity.h"
#include "sprite.h"
#include "timing.h"
#include "vector.h"

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Surface SDL_Surface;
typedef struct SDL_Texture SDL_Texture;

typedef struct game_resource_s game_resource_t;

typedef enum {
    ES_STARTUP,
    ES_PLAY,
    ES_QUIT,
} engine_state_t;

typedef struct engine_s {
    int32_t adapter_index;

    SDL_Window* window;
    SDL_Renderer* renderer;
    // SDL_Surface* scr_surface;
    // SDL_Texture* scr_texture;

    int32_t wnd_width, wnd_height;
    int32_t scr_width, scr_height;
    float scr_scale_x, scr_scale_y;
    rect_t scr_bounds;
    vec2f_t mouse_pos;

    float target_fps;
    double target_frametime;
    int32_t frame_count;

    engine_state_t state;
    bool debug;

    entity_t* ent_list;
    game_resource_t** game_resources;
} engine_t;

extern engine_t* engine;

bool sys_init(engine_t* eng);
void sys_refresh(engine_t* eng);
void sys_shutdown(engine_t* eng);

// void game_res_init(engine_t* eng, const char* assets_path);

#endif