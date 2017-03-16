#ifndef H_MAIN
#define H_MAIN

#include "timing.h"
#include <stdio.h>
#include <SDL.h>

#define ARRAY_SIZE(x) sizeof(x) / sizeof(x[0])
#define TARGET_FPS 60
#define TARGET_FRAMETIME(f) 1/(f) * 1000

typedef enum {
	ES_STARTUP,
	ES_PLAY,
	ES_QUIT
} engine_state_t;

struct _engine_s {
	SDL_Window *window;
	SDL_Renderer *renderer;
	engine_state_t state;
	timing_t timing;
};

typedef struct _engine_s engine_t;

inline void engine_lockfps(double frame_time, double fps)
{		
	if (frame_time < (double)TARGET_FRAMETIME(fps)) {
		while (frame_time < (double)TARGET_FRAMETIME(fps)) {
			frame_time += 0.0001;
		}
	}
	/*printf("frame time: %fms\n", frame_time);*/
}

#endif
