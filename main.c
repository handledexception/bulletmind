/* 		Bulletmind
		(c) 2017 Paul Hindt
		v0.1.101117a
*/

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


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

double engine_time;
int frame_count;

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
	
	font_init(engine->renderer, "C:\\Users\\paulh\\Documents\\Code\\Bulletmind\\assets\\7px_font.tga");
	
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
				
#ifdef DEBUG_PRINT
				char timebuf[256];
				_strtime_s(timebuf, 128);
				font_print(engine->renderer, 25, 10, 1.0, "Engine Time: %f", engine_time);
				font_print(engine->renderer, 25, 20, 1.0, "Frametime: %f", dt);
				font_print(engine->renderer, 25, 30, 1.0, "Frame Count: %d", frame_count);
				font_print(engine->renderer, 25, 40, 1.0, "Time: %s", timebuf);
#endif				
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
			if (dt > TARGET_FRAMETIME(30)) { dt = TARGET_FRAMETIME(30); }
		} while (dt < target_frametime);
		//printf("%f\n", dt);
		SDL_RenderPresent(engine->renderer);
		frame_count++;		
	}

	font_shutdown();
	sys_shutdown(engine);	

	free(engine);
	engine = NULL;
	return 0;
}