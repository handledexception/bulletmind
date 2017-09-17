/* 		Bulletmind
		(c) 2017 Paul Hindt
		v0.1.030817a
*/

#include "bitwise.h"
#include "command.h"
#include "entity.h"
#include "input.h"
#include "main.h"
#include "system.h"
#include "timing.h"
#include "vector.h"

#include <stdio.h>
#include <string.h>

double engine_time = 0.0;

void drawrect_centered(SDL_Renderer *rend, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{		
	recti32_t rec = { 
		x - (w / 2), 
		y - (w / 2), 
		w, h
	};
	
	SDL_SetRenderDrawColor(rend, r, g, b, a);
	SDL_RenderFillRect(rend, (const SDL_Rect *)&rec);
}

int main(int argc, char *argv[])
{	
	engine_t *engine = (engine_t *)malloc(sizeof(engine_t));
	if (engine == NULL) { return -1; }	
	
	sys_init(engine);		
	
	// main loop	
	engine->state = ES_PLAY;	
	while(engine->state != ES_QUIT) {		
		double frame_start = timing_getmillisec();
		
		switch(engine->state) {
			case ES_STARTUP:
				break;
			case ES_PLAY:
				sys_refresh();
				
				SDL_SetRenderDrawColor(engine->renderer, 0x00, 0x00, 0x00, 0xFF);
				SDL_RenderClear(engine->renderer);
				
				drawrect_centered(engine->renderer, (WINDOW_WIDTH / 2), (WINDOW_HEIGHT / 2), 20, 20, 0xff, 0x00, 0x00, 0xff);
		
				if (cmd_getstate(CMD_QUIT) == true) { engine->state = ES_QUIT; }
				break;
			case ES_QUIT:
				break;
		}
		
		SDL_RenderPresent(engine->renderer);
		
		engine_time = timing_getmillisec() - frame_start;
		engine_lockfps(engine_time, TARGET_FPS);
		
		engine->frame_count++;
	}

	sys_shutdown(engine);	

	free(engine);
	engine = NULL;
	return 0;
}