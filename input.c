//#include "bitwise.h"
#include "command.h"
#include "input.h"
#include <stdio.h>
#include <string.h>
#include <SDL.h>

#define MAX_KEYS 512
#define MAX_MBUTTONS 64

typedef struct {
	uint8_t state;
	int32_t cmd;
} key_t;

typedef struct {
	uint8_t state;
	int32_t cmd;
} mbutton_t;

static key_t *array_keys = NULL;
size_t sz_arraykeys = sizeof(key_t) * MAX_KEYS;

static mbutton_t *array_mbuttons = NULL;
size_t sz_arraymbuttons = sizeof(mbutton_t) * MAX_MBUTTONS;

void in_init()
{	
	array_keys = (key_t *)malloc(sz_arraykeys);
	if (array_keys != NULL) { memset(array_keys, 0, sz_arraykeys); }
	
	array_mbuttons = (mbutton_t *)malloc(sz_arraymbuttons);
	if (array_mbuttons != NULL) { memset(array_mbuttons, 0, sz_arraymbuttons); }

	in_setkeybind(SDL_SCANCODE_ESCAPE, CMD_QUIT);
	in_setkeybind(SDL_SCANCODE_W, CMD_PLAYER_UP);
	in_setkeybind(SDL_SCANCODE_S, CMD_PLAYER_DOWN);
	in_setkeybind(SDL_SCANCODE_A, CMD_PLAYER_LEFT);
	in_setkeybind(SDL_SCANCODE_D, CMD_PLAYER_RIGHT);
	//in_setkeybind(SDL_SCANCODE_X, 0x7f); // fail case for testing

	in_setmbuttonbind(SDL_BUTTON_LEFT, CMD_PLAYER_PRIMARY_FIRE);
	in_setmbuttonbind(SDL_BUTTON_RIGHT, CMD_PLAYER_ALTERNATE_FIRE);

	printf("in_init OK\n");
}

uint32_t in_refresh()
{	
	return 0;
}

void in_shutdown()
{
	if (array_keys) {
		free(array_keys);
		array_keys = NULL;
	}

	if (array_mbuttons) {
		free(array_mbuttons);
		array_mbuttons = NULL;
	}

	printf("in_shutdown OK\n");
}

void in_setkeystate(uint16_t key, uint8_t state)
{
	if (array_keys != NULL) {
		if (array_keys[key].state != state) { 
			array_keys[key].state = state;
		}

		int32_t cmd = (array_keys[key].state > 0) ? array_keys[key].cmd : -array_keys[key].cmd;		
		if (cmd > 0) { *array_cmds |= cmd; }
		if (cmd < 0) { *array_cmds &= ~cmd; }
		//printf("in_setkeystate - [key:state:cmd] %d : %d : %d\n", key, state, cmd);		
	}
}

uint8_t	in_getkeystate(uint16_t key)
{
	uint8_t state = 0;
	if (array_keys != NULL) {
		state = array_keys[key].state;
	}
	
	return state;
}

bool in_setkeybind(uint16_t key, int32_t cmd)
{
	bool found_cmd = false;
	const char *cmd_name = NULL;

	for (uint32_t i = 0; i < COMMAND_COUNT; i++) {
		if (COMMAND_LIST[i] == cmd) {
			cmd_name = COMMAND_NAMES[i];
			found_cmd = true;
		}
	}

	if (!found_cmd) {		
		printf("in_setkeybind - error binding Key \"%s\", unknown Command ID \"%d\"!\n", SDL_GetScancodeName(key), cmd);
		return false;
	} 
	
	if (array_keys) {
		array_keys[key].cmd = cmd;
		printf("in_setkeybind - successfully bound Key \"%s\" to Command \"%s\"\n", SDL_GetScancodeName(key), cmd_name);
	}
	
	return true;
}

bool in_setmbuttonbind(uint8_t button, int32_t cmd)
{
	bool found_cmd = false;
	const char *cmd_name = NULL;

	for (uint32_t i = 0; i < COMMAND_COUNT; i++) {
		if (COMMAND_LIST[i] == cmd) {
			cmd_name = COMMAND_NAMES[i];
			found_cmd = true;
		}
	}

	if (!found_cmd) {		
		printf("in_setmbuttonbind - error binding Button \"%d\", unknown Command ID \"%d\"!\n", button, cmd);
		return false;
	} 
	
	if (array_mbuttons) {
		array_mbuttons[button].cmd = cmd;
		printf("in_setmbuttonbind - successfully bound Button \"%d\" to Command \"%s\"\n", button, cmd_name);
	}
	
	return true;
}

void in_setmousebuttonstate(uint8_t button, uint8_t state)
{
	if (array_mbuttons != NULL) {
		if (array_mbuttons[button].state != state) { 
			array_mbuttons[button].state = state;
		}
	}

	int32_t cmd = (array_mbuttons[button].state > 0) ? array_mbuttons[button].cmd : -array_mbuttons[button].cmd;
	if (cmd > 0) { *array_cmds |= cmd; }
	if (cmd < 0) { *array_cmds &= ~cmd; }
	//if (cmd > 0) { bit_set_uint32(array_cmds, cmd); /* printf("in_setkeystate - +cmd\n"); */ } else
	//if (cmd < 0) { bit_clear_uint32(array_cmds, abs(cmd)); /* printf("in_setkeystate - -cmd\n");*/ }
	//printf("in_setmousebuttonstate - [button:state:cmd] %d : %d : %d\n", button, state, cmd);
}