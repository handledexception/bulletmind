#include "input.h"

#include <SDL.h>

#include <stdio.h>
#include <string.h>

#define MAX_KEYS 285
#define MAX_MBUTTONS 64

typedef struct {
	u8 state;
	command_t cmd;
} key_t;

typedef struct {
	u8 state;
	command_t cmd;
} mbutton_t;

static key_t* array_keys = NULL;
size_t sz_arraykeys = sizeof(key_t) * MAX_KEYS;

static mbutton_t* array_mbuttons = NULL;
size_t sz_arraymbuttons = sizeof(mbutton_t) * MAX_MBUTTONS;

void inp_init()
{
	array_keys = (key_t*)malloc(sz_arraykeys);
	if (array_keys != NULL) {
		memset(array_keys, 0, sz_arraykeys);
	}

	array_mbuttons = (mbutton_t*)malloc(sz_arraymbuttons);
	if (array_mbuttons != NULL) {
		memset(array_mbuttons, 0, sz_arraymbuttons);
	}

	inp_set_key_bind(SDL_SCANCODE_ESCAPE, kCommandQuit);
	inp_set_key_bind(SDL_SCANCODE_W, kCommandPlayerUp);
	inp_set_key_bind(SDL_SCANCODE_S, kCommandPlayerDown);
	inp_set_key_bind(SDL_SCANCODE_A, kCommandPlayerLeft);
	inp_set_key_bind(SDL_SCANCODE_D, kCommandPlayerRight);
	inp_set_key_bind(SDL_SCANCODE_LSHIFT, kCommandPlayerSpeed);
	inp_set_key_bind(SDL_SCANCODE_F5, kCommandSetFpsHigh);
	inp_set_key_bind(SDL_SCANCODE_F6, kCommandSetFpsLow);
	inp_set_key_bind(SDL_SCANCODE_F1, kCommandDebugMode);
	//inp_set_key_bind(SDL_SCANCODE_X, 0x7f); // fail case for testing

	inp_set_mouse_bind(SDL_BUTTON_LEFT, kCommandPlayerPrimaryFire);
	inp_set_mouse_bind(SDL_BUTTON_RIGHT, kCommandPlayerAltFire);

	printf("inp_init OK\n");
}

u32 inp_refresh()
{
	return 0;
}

void inp_shutdown()
{
	if (array_keys) {
		free(array_keys);
		array_keys = NULL;
	}

	if (array_mbuttons) {
		free(array_mbuttons);
		array_mbuttons = NULL;
	}

	printf("inp_shutdown OK\n");
}

void inp_set_key_state(u16 key, u8 state)
{
	if (array_keys != NULL) {
		if (array_keys[key].state != state) {
			array_keys[key].state = state;
		}

		const command_t cmd = array_keys[key].cmd;

		const bool cmd_active = (array_keys[key].state > 0);

		if (cmd < kCommandUnknown)
			kActiveCommands[cmd] = cmd_active;
	}
}

u8 inp_get_key_state(u16 key)
{
	u8 state = 0;
	if (array_keys != NULL) {
		state = array_keys[key].state;
	}

	return state;
}

bool inp_set_key_bind(u16 key, command_t cmd)
{
	bool found_cmd = false;
	const char* cmd_name = NULL;

	cmd_name = cmd_type_to_string(cmd);
	if (cmd < kCommandUnknown)
		found_cmd = true;

	if (!found_cmd) {
		printf("inp_set_key_bind - error binding Key \"%s\", unknown Command ID \"%d\"!\n",
		       SDL_GetScancodeName(key), cmd);
		return false;
	}

	if (array_keys) {
		array_keys[key].cmd = cmd;
		//printf("scancode = %d\n", key);
		printf("inp_set_key_bind - successfully bound Key \"%s\" to Command \"%s\"\n",
		       SDL_GetScancodeName(key), cmd_name);
	}

	return true;
}

bool inp_set_mouse_bind(u8 button, command_t cmd)
{
	bool found_cmd = false;
	const char* cmd_name = NULL;

	cmd_name = cmd_type_to_string(cmd);
	if (cmd < kCommandUnknown)
		found_cmd = true;

	if (!found_cmd) {
		printf("inp_set_mouse_bind - error binding Button \"%d\", unknown Command ID \"%d\"!\n",
		       button, cmd);
		return false;
	}

	if (array_mbuttons) {
		array_mbuttons[button].cmd = cmd;
		printf("inp_set_mouse_bind - successfully bound Button \"%d\" to Command \"%s\"\n",
		       button, cmd_name);
	}

	return true;
}

u8 inp_get_mouse_state(u16 button)
{
	u8 state = 0;
	if (array_mbuttons != NULL) {
		state = array_mbuttons[button].state;
	}

	return state;
}

void inp_set_mouse_state(u8 button, u8 state)
{
	if (array_mbuttons != NULL) {
		if (array_mbuttons[button].state != state) {
			array_mbuttons[button].state = state;
		}
		const command_t cmd = array_mbuttons[button].cmd;

		const bool cmd_active = (array_mbuttons[button].state > 0);
				
		if (cmd < kCommandUnknown)
			kActiveCommands[cmd] = cmd_active;

		printf("inp_set_mouse_state - [button:state:cmd] %d : %d : %d\n", button, state, cmd);
	}
}
