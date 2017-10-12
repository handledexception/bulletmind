/* 		Bulletmind
		(c) 2017 Paul Hindt
		v0.1.030817a
*/
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

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int main(int argc, char* argv[])
{	
	engine_t *engine = (engine_t *)malloc(sizeof(engine_t));
	if (engine == NULL) { return -1; }	
	
	if (sys_init(engine) == 0) {
		engine->state = ES_PLAY;
	}
	else {
		printf("sys_init failed!\n");
		return -1;
	}	
	
	font_init(engine->renderer, "G:\\bulletmind\\assets\\7px_font.tga");
	
	double dt = 0.0;
	double target_frametime = TARGET_FRAMETIME(60);	
	recti32_t scr = { 0, 0, engine->scr_width, engine->scr_height };
	
	// main loop	
	while(engine->state != ES_QUIT) {		
		double frame_start = timing_getsec();	

		switch(engine->state) {
			case ES_STARTUP:
				break;
			case ES_PLAY:		

				SDL_SetRenderDrawColor(engine->renderer, 0x00, 0x00, 0x00, 0xFF);
				SDL_RenderClear(engine->renderer);			
				
				font_print(engine->renderer, 25, 25, 1.0, "Frametime: %f", dt);
				font_print(engine->renderer, 25, 45, 1.0, "timing_getsec: %f", timing_getsec());
		
				sys_refresh();				
				ent_refresh(engine->renderer, dt, &scr);
				
				if (cmd_getstate(CMD_QUIT) == true) { engine->state = ES_QUIT; }
				if (cmd_getstate(CMD_SET_FPS_60) == true) { target_frametime = TARGET_FRAMETIME(60); }
				if (cmd_getstate(CMD_SET_FPS_10) == true) { target_frametime = TARGET_FRAMETIME(10); }
				break;
			case ES_QUIT:
				break;
		}			
		
		do { 
			dt = timing_getsec() - frame_start;
		} while (dt < target_frametime);
		// printf("%f\n", dt);
		SDL_RenderPresent(engine->renderer);
		engine->frame_count++;		
	}

	font_shutdown();
	sys_shutdown(engine);	

	free(engine);
	engine = NULL;
	return 0;
}