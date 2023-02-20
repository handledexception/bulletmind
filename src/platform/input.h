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
#include "platform/gamepad.h"
#include "platform/keyboard.h"
#include "platform/mouse.h"
#include "platform/keyboard-scancode.h"
#include "math/vec2.h"

#include <stdlib.h>

// #include <SDL.h>

#define MAX_VIRTUAL_BUTTONS \
	(MAX_KEYBOARD_KEYS + MAX_MOUSE_BUTTONS + GAMEPAD_BUTTON_MAX)

typedef enum { kInputModeGame = 0, kInputModeConsole = 1 } input_mode_t;

typedef enum {
	kInputNone = -1,
	kInputKeyboard = 0,
	kInputMouse = 1,
	kInputJoystick = 2,
	kInputGamepad = 3,
	kInputMax = 4
} input_type_t;

typedef struct gui_event gui_event_t;
// typedef struct kbkey_s {
// 	u64 timestamp; // timestamp of last key state change
// 	u16 scancode;  // keyboard scancode
// 	u8 state;      // key up/down state
// } struct keyboard_key;

// typedef struct mbutton_s {
// 	u64 timestamp; // timestamp of last mouse button state change
// 	u16 button;    // button number
// 	u8 state;      // button up/down state
// } mouse_button_t;

// typedef struct mouse_s {
// 	vec2i_t screen_pos; // mouse position on the screen
// 	vec2i_t window_pos; // mouse position within the window
// 	vec2i_t wheel;      // mouse wheel vector
// 	mouse_button_t buttons[MAX_MOUSE_BUTTONS]; // array of mouse buttons
// } mouse_t;

typedef struct virtual_button_s {
	const char* name; // display name
	u8 state;         // 0 = up/released, 1 = down/pressed
	mouse_button_t* mouse_button;
	keyboard_key_t* keyboard_key;
	gamepad_button_t* pad_button;
	bool toggled;
} virtual_button_t;

struct input_state {
	gamepad_t gamepads[MAX_GAMEPADS];       /* array of gamepad states */
	keyboard_key_t keys[MAX_KEYBOARD_KEYS]; /* array of keyboard key states */
	mouse_t mouse;                          /* mouse state */
	virtual_button_t buttons[MAX_VIRTUAL_BUTTONS];
	input_mode_t mode;
};

struct input_state* inp_new();
void inp_free(struct input_state* inputs);
result inp_init(struct input_state* inputs);
// void inp_refresh_mouse(mouse_t* mouse, f32 scale_x, f32 scale_y);
void inp_read_gui_event(struct input_state* inputs, const gui_event_t* evt);
void inp_shutdown(struct input_state* inputs);

bool inp_init_gamepads(struct input_state* inputs);

void inp_set_key_state(keyboard_key_t* keys, u16 scancode, u8 state);
u8 inp_get_key_state(keyboard_key_t* keys, u16 scancode);

void inp_update_mouse(struct input_state* inputs, const mouse_t* mouse);
// Gamepad deadzone
// https://www.gamasutra.com/blogs/JoshSutphin/20130416/190541/Doing_Thumbstick_Dead_Zones_Right.php
// const char* inp_gamepad_button_kind_to_string(gamepad_button_kind_t kind);
// gamepad_button_kind_t
// inp_gamepad_button_kind_from_sdl(const SDL_GameControllerButton button);
// gamepad_axis_kind_t
// inp_gamepad_axis_kind_from_sdl(const SDL_GameControllerAxis axis);
// bool inp_enumerate_gamepad_buttons(struct gamepad* gamepad);
// bool inp_enumerate_gamepad_axes(struct gamepad* gamepad);
// void inp_set_gamepad_button_state(struct gamepad* gamepad,
// 				  gamepad_button_kind_t button, u8 state);
// u8 inp_get_gamepad_button_state(struct gamepad* gamepad,
// 				gamepad_button_kind_t button);
// void inp_set_gamepad_axis_value(struct gamepad* gamepad,
// 				gamepad_axis_kind_t axis, u16 value);
// s16 inp_get_gamepad_axis_value(struct gamepad* gamepad,
// 			       gamepad_axis_kind_t axis);
