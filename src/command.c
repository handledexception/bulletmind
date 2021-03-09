#define _CRT_SECURE_NO_WARNINGS

#include "command.h"
#include "entity.h"
#include "input.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

entity_t *entities;

void cmd_init(void)
{
	// for (size_t cdx = (size_t)kCommandFirst; cdx < (size_t)kCommandMax; cdx++)
	// 	kActiveCommands[(command_t)cdx] = false;
	printf("cmd_init OK\n");
}

//TODO(paulh): Use the bitfield macros for this instead
bool cmd_get_state(input_state_t *inputs, const command_t cmd)
{
	virtual_button_t *vb = &inputs->buttons[cmd];
	key_t *key = vb->keyboard_key;
	gamepad_button_t *gb = vb->gamepad_button;
	mouse_button_t *mb = vb->mouse_button;
	u8 key_state = 0;
	u8 mouse_button_state = 0;
	u8 gamepad_button_state = 0;

	if (key)
		key_state = key->state;
	if (mb)
		mouse_button_state = mb->state;
	if (gb)
		gamepad_button_state = gb->state;

	return (bool)(key_state | mouse_button_state | gamepad_button_state);
	// return kActiveCommands[cmd];
}

void cmd_toggle_bool(input_state_t *inputs, const command_t cmd, bool *value)
{
	static bool toggled = false;
	if (cmd_get_state(inputs, cmd) == true) {
		if (toggled == false) {
			if (value != NULL) {
				if (*value == true)
					*value = false;
				else
					*value = true;
			}
			toggled = true;
		}
	} else {
		if (toggled == true)
			toggled = false;
	}
}

void cmd_refresh(engine_t *eng)
{
	// static bool toggled = false;
	if (cmd_get_state(eng->inputs, kCommandQuit) == true) {
		eng->state = kEngineStateQuit;
	}

	cmd_toggle_bool(eng->inputs, kCommandDebugMode, &eng->debug);

	if (cmd_get_state(eng->inputs, kCommandSetFpsHigh) == true) {
		eng->target_frametime = TARGET_FRAMETIME(60);
	}
	if (cmd_get_state(eng->inputs, kCommandSetFpsLow) == true) {
		eng->target_frametime = TARGET_FRAMETIME(10);
	}
}

void cmd_shutdown(void)
{
	printf("cmd_shutdown OK\n");
}

const char *cmd_get_name(const command_t cmd)
{
	static char buffer[256];

	switch (cmd) {
	case kCommandNone:
		break;
	case kCommandPlayerUp:
		strcpy(&buffer[0], "Player Move Up");
		break;
	case kCommandPlayerDown:
		strcpy(&buffer[0], "Player Move Down");
		break;
	case kCommandPlayerLeft:
		strcpy(&buffer[0], "Player Move Left");
		break;
	case kCommandPlayerRight:
		strcpy(&buffer[0], "Player Move Right");
		break;
	case kCommandPlayerSpeed:
		strcpy(&buffer[0], "Player Speed");
		break;
	case kCommandPlayerPrimaryFire:
		strcpy(&buffer[0], "Player Primary Fire");
		break;
	case kCommandPlayerAltFire:
		strcpy(&buffer[0], "Player Alternate Fire");
		break;
	case kCommandSetFpsHigh:
		strcpy(&buffer[0], "High FPS");
		break;
	case kCommandSetFpsLow:
		strcpy(&buffer[0], "Low FPS");
		break;
	case kCommandQuit:
		strcpy(&buffer[0], "Quit Game");
		break;
	case kCommandDebugMode:
		strcpy(&buffer[0], "Set Debug Mode");
		break;
	case kCommandMax:
		break;
	}

	return buffer;
}
