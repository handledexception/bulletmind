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

#include "command.h"

#include "core/types.h"
#include "core/scancodes.h"

#include "math/vec2.h"

#include <stdlib.h>

#include <SDL.h>

#define KEY_DOWN true
#define KEY_UP false

#define MAX_GAMEPADS 8
#define MAX_GAMEPAD_BUTTONS 16
#define MAX_GAMEPAD_AXES 6
#define MAX_MOUSE_BUTTONS 16
#define MAX_KEYBOARD_KEYS kScancodeMax
#define MAX_VIRTUAL_BUTTONS	(MAX_KEYBOARD_KEYS + MAX_MOUSE_BUTTONS + MAX_GAMEPAD_BUTTONS)

typedef enum {
	kInputModeGame = 0,
	kInputModeConsole = 1
} input_mode_t;

typedef enum {
	kInputNone = -1,
	kInputKeyboard = 0,
	kInputMouse = 1,
	kInputJoystick = 2,
	kInputGamepad = 3,
	kInputMax = 4
} input_type_t;

typedef enum {
	kGamepadAxisNone = -1,
	kGamepadAxisLeftStickX = 0,
	kGamepadAxisLeftStickY = 1,
	kGamepadAxisRightStickX = 2,
	kGamepadAxisRightStickY = 3,
	kGamepadAxisLeftTrigger = 4,
	kGamepadAxisRightTrigger = 5,
	kGamepadAxisMax = 6
} gamepad_axis_kind_t;

typedef enum {
	kGamepadButtonNone = -1,
	kGamepadButtonA = 0,
	kGamepadButtonB = 1,
	kGamepadButtonX = 2,
	kGamepadButtonY = 3,
	kGamepadButtonBack = 4,
	kGamepadButtonGuide = 5,
	kGamepadButtonStart = 6,
	kGamepadButtonLeftStick = 7,
	kGamepadButtonRightStick = 8,
	kGamepadButtonLeftShoulder = 9,
	kGamepadButtonRightShoulder = 10,
	kGamepadButtonDPadUp = 11,
	kGamepadButtonDPadDown = 12,
	kGamepadButtonDPadLeft = 13,
	kGamepadButtonDPadRight = 14,
	kGamepadButtonMax = 15
} gamepad_button_kind_t;

typedef struct gamepad_axis_s {
	s32 index;
	const char* name;
	s16 value;
	u64 timestamp;
	gamepad_axis_kind_t kind;
} gamepad_axis_t;

typedef struct gamepad_button_s {
	s32 index;
	const char* name;
	const char* alt_name;
	gamepad_button_kind_t kind;
	u8 state;
	u64 timestamp;
} gamepad_button_t;

typedef struct gamepad_s {
	const char* name;
	void* instance;
	s32 index;
	bool is_connected;
	u16 product_id;
	u16 vendor_id;
	u16 version;
	gamepad_button_t buttons[MAX_GAMEPAD_BUTTONS];
	gamepad_axis_t axes[kGamepadAxisMax];
} gamepad_t;

typedef struct kbkey_s {
	u64 timestamp; // timestamp of last key state change
	u16 scancode;  // keyboard scancode
	u8 state;      // key up/down state
} kbkey_t;

typedef struct mbutton_s {
	u64 timestamp; // timestamp of last mouse button state change
	u16 button;    // button number
	u8 state;      // button up/down state
} mouse_button_t;

typedef struct mouse_s {
	vec2i_t screen_pos; // mouse position on the screen
	vec2i_t window_pos; // mouse position within the window
	vec2i_t wheel;      // mouse wheel vector
	mouse_button_t buttons[MAX_MOUSE_BUTTONS]; // array of mouse buttons
} mouse_t;

typedef struct virtual_button_s {
	const char* name; // display name
	u8 state;         // 0 = up/released, 1 = down/pressed
	mouse_button_t* mouse_button;
	kbkey_t* keyboard_key;
	gamepad_button_t* gamepad_button;
	bool toggled;
} virtual_button_t;

typedef struct input_state_s {
	gamepad_t gamepads[MAX_GAMEPADS]; // array of gamepad states
	kbkey_t keys[MAX_KEYBOARD_KEYS];     // array of keyboard key states
	mouse_t mouse;                    // mouse state
	virtual_button_t buttons[MAX_VIRTUAL_BUTTONS];
	input_mode_t mode;
} input_state_t;

bool inp_init(input_state_t* inputs);
void inp_refresh_mouse(mouse_t* mouse, f32 scale_x, f32 scale_y);
void inp_refresh_pressed(input_state_t* inputs, const SDL_Event* evt);
void inp_shutdown(input_state_t* inputs);

bool inp_init_keyboard(input_state_t* inputs);
bool inp_init_mouse(input_state_t* inputs);
bool inp_init_gamepads(input_state_t* inputs);

void inp_set_key_state(kbkey_t* keys, u16 scancode, u8 state);
u8 inp_get_key_state(kbkey_t* keys, u16 scancode);

void inp_set_mouse_pos(mouse_t* mouse, const vec2i_t scr, const vec2i_t wnd);
void inp_set_mouse_button_state(mouse_t* mouse, u16 button, u8 state);
u8 inp_get_mouse_button_state(mouse_t* mouse, u16 button);

// Gamepad deadzone
// https://www.gamasutra.com/blogs/JoshSutphin/20130416/190541/Doing_Thumbstick_Dead_Zones_Right.php
const char* inp_gamepad_button_kind_to_string(gamepad_button_kind_t kind);
gamepad_button_kind_t
inp_gamepad_button_kind_from_sdl(const SDL_GameControllerButton button);
gamepad_axis_kind_t
inp_gamepad_axis_kind_from_sdl(const SDL_GameControllerAxis axis);
bool inp_enumerate_gamepad_buttons(gamepad_t* gamepad);
bool inp_enumerate_gamepad_axes(gamepad_t* gamepad);
void inp_set_gamepad_button_state(gamepad_t* gamepad,
				  gamepad_button_kind_t button, u8 state);
u8 inp_get_gamepad_button_state(gamepad_t* gamepad,
				gamepad_button_kind_t button);
void inp_set_gamepad_axis_value(gamepad_t* gamepad, gamepad_axis_kind_t axis,
				u16 value);
s16 inp_get_gamepad_axis_value(gamepad_t* gamepad, gamepad_axis_kind_t axis);

bool inp_bind_virtual_key(input_state_t* inputs, command_t cmd, u16 scancode);
bool inp_bind_virtual_mouse_button(input_state_t* inputs, command_t cmd,
				   u16 mouse_button);
bool inp_bind_virtual_gamepad_button(input_state_t* inputs, command_t cmd,
				     u32 gamepad, gamepad_button_kind_t button);
