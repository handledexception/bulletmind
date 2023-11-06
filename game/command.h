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

#pragma once

#include "core/types.h"

typedef enum {
	kCommandFirst = 0,
	kCommandMoveForward = kCommandFirst,
	kCommandMoveBack = 1,
	kCommandMoveLeft = 2,
	kCommandMoveRight = 3,
	kCommandMoveUp = 4,
	kCommandMoveDown = 5,
	kCommandPlayerSpeed = 6,
	kCommandPlayerPrimaryFire = 7,
	kCommandPlayerAltFire = 8,
	kCommandSetFpsHigh = 9,
	kCommandSetFpsLow = 10,
	kCommandQuit = 11,
	kCommandDebugMode = 12,
	kCommandConsole = 13,
	kCommandToggleFullscreen = 14,
	kCommandNone = 15,
	kCommandMax = kCommandNone
} command_t;

struct input_state;

const char* cmd_get_name(command_t cmd);
bool inp_cmd_get_state(struct input_state* inputs, command_t cmd);
void inp_cmd_toggle(struct input_state* inputs, command_t cmd, bool* value);
bool inp_bind_virtual_key(struct input_state* inputs, command_t cmd, u16 scancode);
bool inp_bind_virtual_mouse_button(struct input_state* inputs, command_t cmd,
				   u16 mouse_button);
// bool inp_bind_virtual_gamepad_button(struct input_state* inputs, command_t cmd,
// 				     u32 gamepad, gamepad_button_kind_t button);