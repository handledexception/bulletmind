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
#include "command.h"

#include "core/logger.h"
#include "core/video.h"
#include "platform/input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* cmd_get_name(command_t cmd)
{
	static char buffer[256];

	switch (cmd) {
	case kCommandMoveForward:
		strcpy(&buffer[0], "Player Move Up");
		break;
	case kCommandMoveBack:
		strcpy(&buffer[0], "Player Move Down");
		break;
	case kCommandMoveLeft:
		strcpy(&buffer[0], "Player Move Left");
		break;
	case kCommandMoveRight:
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

bool inp_bind_virtual_key(struct input_state* inputs, command_t cmd, u16 scancode)
{
	if (cmd < MAX_VIRTUAL_BUTTONS && scancode < SCANCODE_MAX) {
		inputs->buttons[cmd].state = 0;
		inputs->buttons[cmd].keyboard_key = &inputs->keys[scancode];
		logger(LOG_INFO, "Command `%s` bound to key `%d`",
		       cmd_get_name(cmd), scancode);
		return true;
	}

	return false;
}

bool inp_bind_virtual_mouse_button(struct input_state* inputs, command_t cmd,
				   u16 mouse_button)
{
	if (!inputs)
		return false;

	if (cmd < MAX_VIRTUAL_BUTTONS && mouse_button < MAX_MOUSE_BUTTONS) {
		inputs->buttons[cmd].state = 0;
		inputs->buttons[cmd].mouse_button =
			&inputs->mouse.buttons[mouse_button];
		logger(LOG_INFO, "Command %s bound to mouse button %d",
		       cmd_get_name(cmd), mouse_button);
		return true;
	}

	return false;
}

bool inp_bind_virtual_gamepad_button(struct input_state* inputs, command_t cmd,
				     u32 gamepad, gamepad_button_kind_t button)
{
	if (cmd < MAX_VIRTUAL_BUTTONS && button < GAMEPAD_BUTTON_MAX) {
		inputs->buttons[cmd].state = 0;
		inputs->buttons[cmd].pad_button =
			&inputs->gamepads[gamepad].buttons[button];

		logger(LOG_INFO,
		       "Command %s bound to gamepad %d/button %d (%s)",
		       cmd_get_name(cmd), gamepad, button,
		       inputs->gamepads[gamepad].buttons[button].name);

		return true;
	}

	return false;
}

bool inp_cmd_get_state(struct input_state* inputs, command_t cmd)
{
	if (cmd > MAX_VIRTUAL_BUTTONS)
		return;
	bool inputs_state = false;
	// special case console so we can toggle console back off again
	if (inputs->mode == kInputModeGame ||
	    (inputs->mode == kInputModeConsole && cmd == kCommandConsole)) {
		virtual_button_t* vb = &inputs->buttons[cmd];
		if (!vb)
			return false;

		keyboard_key_t* key = vb->keyboard_key;
		gamepad_button_t* gb = vb->pad_button;
		mouse_button_t* mb = vb->mouse_button;
		u8 key_state = 0;
		enum mouse_button_state mb_state = 0;
		u8 gamepad_button_state = 0;

		if (key)
			key_state = key->state;
		if (mb)
			mb_state = mb->state;
		if (gb)
			gamepad_button_state = gb->state;

		inputs_state = (bool)(key_state | mb_state |
				      gamepad_button_state);
	}

	return inputs_state;
}

void inp_cmd_toggle(struct input_state* inputs, command_t cmd, bool* value)
{
	if (cmd > MAX_VIRTUAL_BUTTONS)
		return;
	bool toggled = inputs->buttons[cmd].toggled;
	if (inp_cmd_get_state(inputs, cmd)) {
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
