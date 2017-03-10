/* 		Bulletmind
		(c) 2017 Paul Hindt
		v0.1.030817a
*/

#include "bitwise.h"
#include "command.h"
#include "input.h"
#include "main.h"
#include "system.h"
#include <SDL.h>
#include <stdio.h>

int main(int argc, char *argv[])
{	
	engine_t *engine = (engine_t *)malloc(sizeof(engine_t));
	if (engine == NULL) { return -1; }

	engine->state = ES_STARTUP;
	sys_init(engine);
		
	// main loop
	engine->state = ES_PLAY;	
	while(engine->state != ES_QUIT) {		
		switch(engine->state) {
			case ES_STARTUP:
				break;
			case ES_PLAY:
				sys_refresh();
				if (cmd_getstate(CMD_PLAYER_UP) == true) { printf("PLAYER UP\n"); };	
				if (cmd_getstate(CMD_QUIT) == true) { engine->state = ES_QUIT; }
				break;
			case ES_QUIT:
				break;
		}
		engine_lockfps(engine, TARGET_FPS);
	}
	sys_shutdown(engine);	
	free(engine);
	
	return 0;
}