#define _CRT_SECURE_NO_WARNINGS

#include "command.h"
#include "entity.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

entity_t* entities;

void cmd_init(void)
{
	for (size_t cdx = (size_t)kCommandFirst; cdx < (size_t)kCommandMax; cdx++)
		kActiveCommands[(command_t)cdx] = false;
	printf("cmd_init OK\n");
}

//TODO(paulh): Use the bitfield macros for this instead
bool cmd_getstate(const command_t cmd)
{
	return kActiveCommands[cmd];
}

void cmd_toggle_bool(const command_t cmd, bool* value)
{
	static bool toggled = false;
	if (cmd_getstate(cmd) == true) {
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

void cmd_refresh(engine_t* eng)
{
	// static bool toggled = false;
	if (cmd_getstate(kCommandQuit) == true) {
		eng->state = ES_QUIT;
	}

	cmd_toggle_bool(kCommandDebugMode, &eng->debug);

	if (cmd_getstate(kCommandSetFpsHigh) == true) {
		eng->target_frametime = TARGET_FRAMETIME(60);
	}
	if (cmd_getstate(kCommandSetFpsLow) == true) {
		eng->target_frametime = TARGET_FRAMETIME(10);
	}
}

void cmd_shutdown(void)
{
	printf("cmd_shutdown OK\n");
}

const char* cmd_type_to_string(const command_t cmd)
{
	static char buffer[4096];

	switch(cmd) {
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
