/*
 * Copyright (c) 2021 Paul Hindt
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "core/logger.h"
#include "core/video.h"

#include "command.h"
#include "entity.h"
#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

entity_t* entities;

void cmd_init(void)
{
	// for (size_t cdx = (size_t)kCommandFirst; cdx < (size_t)kCommandMax; cdx++)
	// 	kActiveCommands[(command_t)cdx] = false;
	logger(LOG_INFO, "cmd_init OK\n");
}

//TODO(paulh): This should probably be redesigned to return the state as an output param
bool cmd_get_state(input_state_t* inputs, command_t cmd)
{
	bool inputs_state = false;

	// special case console so we can toggle console back off again
	if (inputs->mode == kInputModeGame ||
	    (inputs->mode == kInputModeConsole && cmd == kCommandConsole)) {
		virtual_button_t* vb = &inputs->buttons[cmd];
		if (!vb)
			return false;

		kbkey_t* key = vb->keyboard_key;
		gamepad_button_t* gb = vb->gamepad_button;
		mouse_button_t* mb = vb->mouse_button;
		u8 key_state = 0;
		u8 mouse_button_state = 0;
		u8 gamepad_button_state = 0;

		if (key)
			key_state = key->state;
		if (mb)
			mouse_button_state = mb->state;
		if (gb)
			gamepad_button_state = gb->state;

		inputs_state = (bool)(key_state | mouse_button_state |
				      gamepad_button_state);
	}

	return inputs_state;
}

//todo(paul): debounce the keypresses to make toggle work
void cmd_toggle_bool(input_state_t* inputs, command_t cmd, bool* value)
{
	bool toggled = inputs->buttons[cmd].toggled;
	if (cmd_get_state(inputs, cmd)) {
		if (!toggled) {
			*value = !*value;
			inputs->buttons[cmd].toggled = true;
		}
	} else {
		if (toggled) {
			inputs->buttons[cmd].toggled = false;
		}
	}
}

void cmd_refresh(engine_t* eng)
{
	cmd_toggle_bool(eng->inputs, kCommandDebugMode, &eng->debug);

	cmd_toggle_bool(eng->inputs, kCommandConsole, &eng->console);
	if (eng->console) {
		eng->mode = kEngineModeConsole;
		eng->inputs->mode = kInputModeConsole;
	}

	cmd_toggle_bool(eng->inputs, kCommandToggleFullscreen, &eng->fullscreen);
	eng_toggle_fullscreen(eng, eng->fullscreen);

	if (cmd_get_state(eng->inputs, kCommandQuit))
		eng->mode = kEngineModeQuit;

	if (cmd_get_state(eng->inputs, kCommandSetFpsHigh) == true) {
		eng->target_frametime = FRAME_TIME(eng->target_fps);
	}
	if (cmd_get_state(eng->inputs, kCommandSetFpsLow) == true) {
		eng->target_frametime = FRAME_TIME(10);
	}
}

void cmd_shutdown(void)
{
	logger(LOG_INFO, "cmd_shutdown OK\n");
}

const char* cmd_get_name(command_t cmd)
{
	static char buffer[256];

	switch (cmd) {
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
		strcpy(&buffer[0], "Toggle Debug Mode");
		break;
	case kCommandConsole:
		strcpy(&buffer[0], "Toggle Console");
		break;
	default:
	case kCommandNone:
		break;
	}

	return buffer;
}
