/* 		Bulletmind
		(c) 2017 Paul Hindt
		v0.1.030817a
*/

//#include "bitwise.h"
#include "command.h"
#include "entity.h"
#include "imgfile.h"
#include "input.h"
#include "main.h"
#include "system.h"
#include "timing.h"
#include "vector.h"

#include <stdio.h>
#include <string.h>

double engine_time = 0.0;

static void drawrect(SDL_Renderer *rend, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
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
	imgfile_init(".\\assets\\7px_font.tga");	
	return 0;

	engine_t *engine = (engine_t *)malloc(sizeof(engine_t));
	if (engine == NULL) { return -1; }	
	
	if (sys_init(engine) == 0) {
		engine->state = ES_PLAY;
	}
	else {
		printf("sys_init failed!\n");
		return -1;
	}
	
	
	// main loop	
	while(engine->state != ES_QUIT) {		
		double frame_start = timing_getmillisec();

		switch(engine->state) {
			case ES_STARTUP:
				break;
			case ES_PLAY:
				sys_refresh();
				
				SDL_SetRenderDrawColor(engine->renderer, 0x00, 0x00, 0x00, 0xFF);
				SDL_RenderClear(engine->renderer);
				
				ent_refresh(engine->renderer, engine_time);
		
				if (cmd_getstate(CMD_QUIT) == true) { engine->state = ES_QUIT; }
				break;
			case ES_QUIT:
				break;
		}
		
		SDL_RenderPresent(engine->renderer);
		
		engine_time = timing_getmillisec() - frame_start;
		//engine_lockfps(engine_time, TARGET_FPS);
		engine->frame_count++;
	}

	sys_shutdown(engine);	

	free(engine);
	engine = NULL;
	return 0;
}