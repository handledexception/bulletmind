#include "bitwise.h"
#include "command.h"
#include "input.h"
#include <stdio.h>
#include <string.h>
#include <SDL.h>

#define MAX_KEYS 512

typedef struct {
	uint8_t state;
	int32_t cmd;
} key_t;

static key_t *g_array_keys = NULL;
size_t sz_arraykeys = sizeof(key_t) * MAX_KEYS;

void in_init()
{	
	g_array_keys = (key_t *)malloc(sz_arraykeys);
	if (g_array_keys) {	memset(g_array_keys, 0, sz_arraykeys); }
	
	in_setkeybind(SDL_SCANCODE_ESCAPE, CMD_QUIT);
	in_setkeybind(SDL_SCANCODE_W, CMD_PLAYER_UP);
	in_setkeybind(SDL_SCANCODE_S, CMD_PLAYER_DOWN);
	in_setkeybind(SDL_SCANCODE_A, CMD_PLAYER_LEFT);
	in_setkeybind(SDL_SCANCODE_D, CMD_PLAYER_RIGHT);
	printf("in_init OK\n");
}

uint32_t in_refresh()
{	
	return 0;
}

void in_shutdown()
{
	if (g_array_keys) {
		free(g_array_keys);
		g_array_keys = NULL;
	}
}

void in_setkeystate(uint16_t key, uint8_t state)
{
	if (g_array_keys != NULL) {
		if (g_array_keys[key].state != state) { 
			g_array_keys[key].state = state;
		}
		
		//int32_t cmd = g_array_keys[key].cmds[state]; // cmd 0 is key up, cmd 1 is key down
		int32_t cmd = (g_array_keys[key].state > 0) ? g_array_keys[key].cmd : -g_array_keys[key].cmd;
		printf("[key:state:cmd] %d : %d : %d\n", key, state, cmd);
		
		//cmd_exec(cmd);
		if (cmd > 0) { bit_set_uint32(g_array_cmds, cmd); printf("+cmd\n"); } else
		if (cmd < 0) { bit_clear_uint32(g_array_cmds, abs(cmd)); printf("-cmd\n"); }
	}
}

uint8_t	in_getkeystate(uint16_t key)
{
	uint8_t state = 0;
	if (g_array_keys != NULL) {
		state = g_array_keys[key].state;
	}
	
	return state;
}

// todo: print key names and commands to log
void in_setkeybind(uint16_t key, int32_t cmd)
{
	if (g_array_keys) {
		g_array_keys[key].cmd = cmd;		
	}
}