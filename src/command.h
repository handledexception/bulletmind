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

#include "engine.h"

#include "core/c99defs.h"
#include "core/types.h"

typedef struct input_state_s input_state_t;

typedef enum {
	kCommandFirst = 0,
	kCommandPlayerUp = kCommandFirst,
	kCommandPlayerDown = 1,
	kCommandPlayerLeft = 2,
	kCommandPlayerRight = 3,
	kCommandPlayerSpeed = 4,
	kCommandPlayerPrimaryFire = 5,
	kCommandPlayerAltFire = 6,
	kCommandSetFpsHigh = 7,
	kCommandSetFpsLow = 8,
	kCommandQuit = 9,
	kCommandDebugMode = 10,
	kCommandConsole = 11,
	kCommandNone = 12,
	kCommandMax = kCommandNone
} command_t;

void cmd_init(void);
bool cmd_get_state(input_state_t* inputs, command_t cmd);
void cmd_toggle_bool(input_state_t* inputs, command_t cmd, bool* value);
void cmd_refresh(engine_t* engine);
void cmd_shutdown(void);
const char* cmd_get_name(command_t cmd);
