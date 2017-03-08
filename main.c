/* 		Bulletmind
		(c) 2017 Paul Hindt
		v0.1.030817a
*/

#include "bitwise.h"
#include "input.h"
#include "system.h"
#include "timing.h"

#include <SDL.h>
#include <stdio.h>

typedef enum {
	ES_STARTUP,
	ES_PLAY,
	ES_QUIT
} engine_state_t;

engine_state_t engine_state;

int main(int argc, char **argv)
{	
	engine_state = ES_STARTUP;
	sys_init();
		
	// main loop
	engine_state = ES_PLAY;	
	while(engine_state != ES_QUIT) {
		switch(engine_state) {
			case ES_STARTUP:
				break;
			case ES_PLAY:
				sys_refresh();
				if (bit_check_uint32(*array_cmds, CMD_QUIT) > 0) { engine_state = ES_QUIT; }
				if (bit_check_uint32(*array_cmds, CMD_PLAYER_UP) > 0) { printf("PLAYER UP\n"); }
				if (bit_check_uint32(*array_cmds, CMD_PLAYER_DOWN) > 0) { printf("PLAYER DOWN\n"); }
				if (bit_check_uint32(*array_cmds, CMD_PLAYER_LEFT) > 0) { printf("PLAYER LEFT\n"); }
				if (bit_check_uint32(*array_cmds, CMD_PLAYER_RIGHT) > 0) { printf("PLAYER RIGHT\n"); }
				
				
				break;
			case ES_QUIT:
				break;
		}		
	}
	sys_shutdown();	
	
	return 0;
}