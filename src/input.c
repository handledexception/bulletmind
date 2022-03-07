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

#include "input.h"

#include "core/logger.h"
#include "core/memory.h"

#include "platform/platform.h"

#include <stdio.h>
#include <string.h>

bool inp_init(input_state_t* inputs)
{
	if (!inputs)
		return false;

	bool ok = inp_init_gamepads(inputs);
	if (!ok) {
		logger(LOG_ERROR,"Error initializing gamepads!");
		return false;
	}
	logger(LOG_INFO,  "Initialized gamepads OK");

	ok = inp_init_keyboard(inputs);
	if (!ok) {
		logger(LOG_ERROR,"Error initializing keyboard!");
		return ok;
	}
	logger(LOG_INFO,  "Initialized keyboard OK");

	ok = inp_init_mouse(inputs);
	if (!ok) {
		logger(LOG_ERROR,"Error initializing mouse!");
		return ok;
	}
	logger(LOG_INFO,  "Initialized mouse OK");

	for (size_t i = 0; i < kCommandMax; i++) {
		inputs->buttons[i].name = NULL;
		inputs->buttons[i].state = 0;
		inputs->buttons[i].mouse_button = NULL;
		inputs->buttons[i].keyboard_key = NULL;
		inputs->buttons[i].gamepad_button = NULL;
		inputs->buttons[i].toggled = false;
	}

	inp_bind_virtual_key(inputs, kCommandQuit, kScancodeEscape);
	inp_bind_virtual_key(inputs, kCommandPlayerUp, kScancodeW);
	inp_bind_virtual_key(inputs, kCommandPlayerDown, kScancodeS);
	inp_bind_virtual_key(inputs, kCommandPlayerLeft, kScancodeA);
	inp_bind_virtual_key(inputs, kCommandPlayerRight, kScancodeD);
	inp_bind_virtual_key(inputs, kCommandPlayerSpeed, kScancodeLeftShift);
	inp_bind_virtual_key(inputs, kCommandSetFpsHigh, kScancodeF5);
	inp_bind_virtual_key(inputs, kCommandSetFpsLow, kScancodeF6);
	inp_bind_virtual_key(inputs, kCommandDebugMode, kScancodeF1);
	inp_bind_virtual_key(inputs, kCommandConsole, kScancodeGrave);
	inp_bind_virtual_key(inputs, kCommandToggleFullscreen, kScancodeF12);
	inp_bind_virtual_key(inputs, kCommandPlayerPrimaryFire, kScancodeSpace);
	inp_bind_virtual_gamepad_button(inputs, kCommandPlayerUp, 0,
					kGamepadButtonDPadUp);
	inp_bind_virtual_gamepad_button(inputs, kCommandPlayerDown, 0,
					kGamepadButtonDPadDown);
	inp_bind_virtual_gamepad_button(inputs, kCommandPlayerLeft, 0,
					kGamepadButtonDPadLeft);
	inp_bind_virtual_gamepad_button(inputs, kCommandPlayerRight, 0,
					kGamepadButtonDPadRight);
	inp_bind_virtual_gamepad_button(inputs, kCommandPlayerPrimaryFire, 0,
					kGamepadButtonRightShoulder);
	inp_bind_virtual_mouse_button(inputs, kCommandPlayerPrimaryFire,
				      SDL_BUTTON_LEFT);
	inp_bind_virtual_mouse_button(inputs, kCommandPlayerAltFire,
				      SDL_BUTTON_RIGHT);

	logger(LOG_INFO,  "inp_init OK\n");

	return true;
}

void inp_refresh_mouse(mouse_t* mouse, f32 scale_x, f32 scale_y)
{
	int mx = 0;
	int my = 0;
	SDL_GetMouseState(&mx, &my);
	f32 fmx = (f32)mx;
	f32 fmy = (f32)my;
	fmx /= scale_x;
	fmy /= scale_y;

	vec2i_t mouse_window_pos;
	mouse_window_pos.x = (s32)fmx;
	mouse_window_pos.y = (s32)fmy;

	vec2i_t mouse_screen_pos;
	SDL_GetGlobalMouseState(&mouse_screen_pos.x, &mouse_screen_pos.y);

	inp_set_mouse_pos(mouse, mouse_screen_pos, mouse_window_pos);
}

void inp_refresh_pressed(input_state_t* inputs, const SDL_Event* evt)
{
	if (evt) {
		switch (evt->type) {
		case SDL_KEYDOWN:
			inp_set_key_state(&inputs->keys[0],
					  evt->key.keysym.scancode, KEY_DOWN);
			break;
		case SDL_KEYUP:
			inp_set_key_state(&inputs->keys[0],
					  evt->key.keysym.scancode, KEY_UP);
			break;
		case SDL_MOUSEBUTTONDOWN:
			//printf("Mouse Button %d DOWN\n", SDL_BUTTON(evt->button.button));
			inp_set_mouse_button_state(&inputs->mouse,
						   evt->button.button,
						   evt->button.state);
			break;
		case SDL_MOUSEBUTTONUP:
			//printf("Mouse Button %d UP\n", SDL_BUTTON(evt->button.button));
			inp_set_mouse_button_state(&inputs->mouse,
						   evt->button.button,
						   evt->button.state);
			break;
		case SDL_CONTROLLERAXISMOTION:
			for (size_t cdx = 0; cdx < BM_MAX_GAMEPADS; cdx++) {
				if (inputs->gamepads[cdx].index ==
				    evt->cdevice.which) {
					inp_set_gamepad_axis_value(
						&inputs->gamepads[cdx],
						(gamepad_axis_kind_t)
							evt->caxis.axis,
						evt->caxis.value);
				}
			}
			//	printf("Controller Axis Motion - Axis: %d | Value: %d\n", evt->caxis.axis, evt->caxis.value);
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			for (size_t cdx = 0; cdx < BM_MAX_GAMEPADS; cdx++) {
				if (inputs->gamepads[cdx].index ==
				    evt->cdevice.which) {
					inp_set_gamepad_button_state(
						&inputs->gamepads[cdx],
						evt->cbutton.button,
						evt->cbutton.state);
				}
			}
			//printf("Controller Button Down - Button: %d\n", evt->cbutton.button);
			break;
		case SDL_CONTROLLERBUTTONUP:
			for (size_t cdx = 0; cdx < BM_MAX_GAMEPADS; cdx++) {
				if (inputs->gamepads[cdx].index ==
				    evt->cdevice.which) {
					inp_set_gamepad_button_state(
						&inputs->gamepads[cdx],
						evt->cbutton.button,
						evt->cbutton.state);
				}
			}
			//printf("Controller Button Up - Button: %d\n", evt->cbutton.button);
			break;
		case SDL_CONTROLLERDEVICEADDED:
			logger(LOG_DEBUG,"Controller Device Added\n");
			break;
		case SDL_CONTROLLERDEVICEREMOVED:
			logger(LOG_DEBUG,"Controller Device Removed\n");
			break;
		case SDL_CONTROLLERDEVICEREMAPPED:
			logger(LOG_DEBUG,"Controller Device Remapped\n");
			break;
		}
	}
}

void inp_shutdown(input_state_t* inputs)
{
	logger(LOG_INFO, "inp_shutdown OK\n");
}

bool inp_init_keyboard(input_state_t* inputs)
{
	for (size_t i = kScancodeUnknown; i < kScancodeMax; i++) {
		inputs->keys[i].scancode = (u16)i;
		inputs->keys[i].state = 0;
		inputs->keys[i].timestamp = 0ULL;
	}

	return true;
}

bool inp_init_mouse(input_state_t* inputs)
{
	inputs->mouse.buttons[SDL_BUTTON_LEFT].button = SDL_BUTTON_LEFT;
	inputs->mouse.buttons[SDL_BUTTON_LEFT].timestamp = 0ULL;
	inputs->mouse.buttons[SDL_BUTTON_LEFT].state = false;

	inputs->mouse.buttons[SDL_BUTTON_MIDDLE].button = SDL_BUTTON_MIDDLE;
	inputs->mouse.buttons[SDL_BUTTON_MIDDLE].timestamp = 0ULL;
	inputs->mouse.buttons[SDL_BUTTON_MIDDLE].state = false;

	inputs->mouse.buttons[SDL_BUTTON_RIGHT].button = SDL_BUTTON_RIGHT;
	inputs->mouse.buttons[SDL_BUTTON_RIGHT].timestamp = 0ULL;
	inputs->mouse.buttons[SDL_BUTTON_RIGHT].state = false;

	inputs->mouse.buttons[SDL_BUTTON_X1].button = SDL_BUTTON_X1;
	inputs->mouse.buttons[SDL_BUTTON_X1].timestamp = 0ULL;
	inputs->mouse.buttons[SDL_BUTTON_X1].state = false;

	inputs->mouse.buttons[SDL_BUTTON_X2].button = SDL_BUTTON_X2;
	inputs->mouse.buttons[SDL_BUTTON_X2].timestamp = 0ULL;
	inputs->mouse.buttons[SDL_BUTTON_X2].state = false;

	return true;
}

bool inp_init_gamepads(input_state_t* inputs)
{
	const int num_joysticks = SDL_NumJoysticks();
	if (num_joysticks == 0)
		return true;

	bool gamepad_err = false;
	for (int gdx = 0; gdx < num_joysticks; gdx++) {
		SDL_GameController* gc = SDL_GameControllerOpen(gdx);
		if (gc == NULL) {
			const char* sdl_err = SDL_GetError();
			if (sdl_err != NULL && sdl_err[0] != '\0')
				logger(LOG_ERROR,
				       "SDL_GameControllerOpen error: %s\n",
				       sdl_err);
			gamepad_err = true;
			continue;
		}

		gamepad_t gamepad;
		memset(&gamepad, 0, sizeof(gamepad_t));
		gamepad.instance = (void*)gc;
		gamepad.name = SDL_GameControllerName(gc);
		gamepad.index = gdx;
		gamepad.is_connected = true;
		gamepad.product_id = SDL_GameControllerGetProduct(gc);
		gamepad.vendor_id = SDL_GameControllerGetVendor(gc);
		gamepad.version = SDL_GameControllerGetProductVersion(gc);

		gamepad_err = inp_enumerate_gamepad_axes(&gamepad);
		if (!gamepad_err)
			gamepad_err = inp_enumerate_gamepad_buttons(&gamepad);

		inputs->gamepads[gdx] = gamepad;

		logger(LOG_INFO,
		       "Gamepad %s connected | Product ID: %d | Vendor ID: %d | Version: %d\n",
		       gamepad.name, gamepad.product_id, gamepad.vendor_id,
		       gamepad.version);
	}

	return !gamepad_err;
}

void inp_set_key_state(kbkey_t* keys, u16 scancode, u8 state)
{
	if (keys) {
		if (keys[scancode].state != state) {
			keys[scancode].state = state;
			keys[scancode].timestamp = os_get_time_ns();
		}
	}
}

u8 inp_get_key_state(kbkey_t* keys, u16 scancode)
{
	u8 state = 0;
	if (keys)
		state = keys[scancode].state;
	return state;
}

void inp_set_mouse_pos(mouse_t* mouse, const vec2i_t scr, const vec2i_t wnd)
{
	if (mouse) {
		mouse->screen_pos = scr;
		mouse->window_pos = wnd;
	}
}

void inp_set_mouse_button_state(mouse_t* mouse, u16 button, u8 state)
{
	if (mouse) {
		mouse->buttons[button].button = button;

		if (mouse->buttons[button].state != state) {
			mouse->buttons[button].state = state;
			mouse->buttons[button].timestamp = os_get_time_ns();
		}
	}
}

u8 inp_get_mouse_button_state(mouse_t* mouse, u16 button)
{
	u8 state = 0;
	if (mouse && mouse->buttons[button].button == button)
		state = mouse->buttons[button].state;
	return state;
}

const char* inp_gamepad_button_kind_to_string(gamepad_button_kind_t kind)
{
	switch (kind) {
	default:
	case kGamepadButtonNone:
	case kGamepadButtonMax:
		return "None";
	case kGamepadButtonA:
		return "A";
	case kGamepadButtonB:
		return "B";
	case kGamepadButtonX:
		return "X";
	case kGamepadButtonY:
		return "Y";
	case kGamepadButtonBack:
		return "Back";
	case kGamepadButtonGuide:
		return "Guide";
	case kGamepadButtonStart:
		return "Start";
	case kGamepadButtonLeftStick:
		return "LS";
	case kGamepadButtonRightStick:
		return "RS";
	case kGamepadButtonLeftShoulder:
		return "Right Shoulder";
	case kGamepadButtonRightShoulder:
		return "Left Shoulder";
	case kGamepadButtonDPadUp:
		return "D-Pad Up";
	case kGamepadButtonDPadDown:
		return "D-Pad Down";
	case kGamepadButtonDPadLeft:
		return "D-Pad Left";
	case kGamepadButtonDPadRight:
		return "D-Pad Right";
	}
}

gamepad_button_kind_t
inp_gamepad_button_kind_from_sdl(const SDL_GameControllerButton button)
{
	gamepad_button_kind_t kind = kGamepadButtonNone;

	switch (button) {
	case SDL_CONTROLLER_BUTTON_INVALID:
		kind = kGamepadButtonNone;
		break;
	case SDL_CONTROLLER_BUTTON_A:
		kind = kGamepadButtonA;
		break;
	case SDL_CONTROLLER_BUTTON_B:
		kind = kGamepadButtonB;
		break;
	case SDL_CONTROLLER_BUTTON_X:
		kind = kGamepadButtonX;
		break;
	case SDL_CONTROLLER_BUTTON_Y:
		kind = kGamepadButtonY;
		break;
	case SDL_CONTROLLER_BUTTON_BACK:
		kind = kGamepadButtonBack;
		break;
	case SDL_CONTROLLER_BUTTON_GUIDE:
		kind = kGamepadButtonGuide;
		break;
	case SDL_CONTROLLER_BUTTON_START:
		kind = kGamepadButtonStart;
		break;
	case SDL_CONTROLLER_BUTTON_LEFTSTICK:
		kind = kGamepadButtonLeftStick;
		break;
	case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
		kind = kGamepadButtonRightStick;
		break;
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		kind = kGamepadButtonLeftShoulder;
		break;
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		kind = kGamepadButtonRightShoulder;
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		kind = kGamepadButtonDPadUp;
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		kind = kGamepadButtonDPadDown;
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		kind = kGamepadButtonDPadLeft;
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		kind = kGamepadButtonDPadRight;
		break;
	case SDL_CONTROLLER_BUTTON_MAX:
		kind = kGamepadButtonMax;
		break;
	default:
		kind = kGamepadButtonNone;
		break;
	}

	return kind;
}

gamepad_axis_kind_t
inp_gamepad_axis_kind_from_sdl(const SDL_GameControllerAxis axis)
{
	gamepad_axis_kind_t kind = kGamepadAxisNone;

	switch (axis) {
	case SDL_CONTROLLER_AXIS_INVALID:
		kind = kGamepadAxisNone;
		break;
	case SDL_CONTROLLER_AXIS_LEFTX:
		kind = kGamepadAxisLeftStickX;
		break;
	case SDL_CONTROLLER_AXIS_LEFTY:
		kind = kGamepadAxisLeftStickY;
		break;
	case SDL_CONTROLLER_AXIS_RIGHTX:
		kind = kGamepadAxisRightStickX;
		break;
	case SDL_CONTROLLER_AXIS_RIGHTY:
		kind = kGamepadAxisRightStickY;
		break;
	case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
		kind = kGamepadAxisLeftTrigger;
		break;
	case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
		kind = kGamepadAxisRightTrigger;
		break;
	case SDL_CONTROLLER_AXIS_MAX:
		kind = kGamepadAxisMax;
		break;
	}

	return kind;
}

bool inp_enumerate_gamepad_buttons(gamepad_t* gamepad)
{
	bool btn_err = false;

	for (s32 bdx = kGamepadButtonA; bdx < kGamepadButtonMax; bdx++) {
		const SDL_GameControllerButton button =
			(SDL_GameControllerButton)bdx;

		const char* button_name =
			SDL_GameControllerGetStringForButton(button);

		const u8 button_state = SDL_GameControllerGetButton(
			(SDL_GameController*)gamepad->instance, button);

		const gamepad_button_kind_t kind =
			inp_gamepad_button_kind_from_sdl(button);

		gamepad->buttons[kind].index = bdx;
		gamepad->buttons[kind].kind = kind;
		gamepad->buttons[kind].state = button_state;
		gamepad->buttons[kind].name = button_name;
		gamepad->buttons[kind].timestamp = os_get_time_ns();

		logger(LOG_INFO,  "Enumerated gamepad button %s\n", button_name);
	}

	return btn_err;
}

bool inp_enumerate_gamepad_axes(gamepad_t* gamepad)
{
	bool axis_err = false;

	for (s32 adx = (s32)kGamepadAxisLeftStickX; adx < (s32)kGamepadAxisMax;
	     adx++) {
		const SDL_GameControllerAxis axis = (SDL_GameControllerAxis)adx;

		const char* axis_name =
			SDL_GameControllerGetStringForAxis(axis);

		const s16 axis_value = SDL_GameControllerGetAxis(
			(SDL_GameController*)gamepad->instance, axis);

		// SDL_GameControllerGetAxis returns 0 on success or failure. Must use SDL_GetError to check for errors.
		if (axis_value == 0) {
			const char* err = SDL_GetError();
			if (err != NULL && err[0] != '\0') {
				logger(LOG_INFO,
				       "SDL_GameControllerGetAxis error: %s\n",
				       err);
				axis_err = true;
			}
		}

		const gamepad_axis_kind_t kind =
			inp_gamepad_axis_kind_from_sdl(axis);
		gamepad->axes[kind].index = adx;
		gamepad->axes[kind].name = axis_name;
		gamepad->axes[kind].value = axis_value;
		gamepad->axes[kind].kind = kind;
		gamepad->axes[kind].timestamp = os_get_time_ns();

		logger(LOG_INFO,  "Enumerated gamepad axis %s\n", axis_name);
	}

	return axis_err;
}

void inp_set_gamepad_button_state(gamepad_t* gamepad,
				  gamepad_button_kind_t button, u8 state)
{
	if (gamepad && gamepad->buttons[button].state != state) {
		gamepad->buttons[button].state = state;
		gamepad->buttons[button].timestamp = os_get_time_ns();
	}
}

u8 inp_get_gamepad_button_state(gamepad_t* gamepad,
				gamepad_button_kind_t button)
{
	u8 state = 0;
	if (gamepad)
		state = gamepad->buttons[button].state;
	return state;
}

void inp_set_gamepad_axis_value(gamepad_t* gamepad, gamepad_axis_kind_t axis,
				u16 value)
{
	if (gamepad)
		gamepad->axes[axis].value = value;
}

s16 inp_get_gamepad_axis_value(gamepad_t* gamepad, gamepad_axis_kind_t axis)
{
	s16 value = 0;
	if (gamepad)
		value = gamepad->axes[axis].value;
	return value;
}

bool inp_bind_virtual_key(input_state_t* inputs, command_t cmd, u16 scancode)
{
	if (cmd < BM_MAX_VIRTUAL_BUTTONS && scancode < kScancodeMax) {
		inputs->buttons[cmd].state = 0;
		inputs->buttons[cmd].keyboard_key = &inputs->keys[scancode];
		logger(LOG_INFO,  "Command %s bound to key %d\n",
		       cmd_get_name(cmd), scancode);
		return true;
	}

	return false;
}

bool inp_bind_virtual_mouse_button(input_state_t* inputs, command_t cmd,
				   u16 mouse_button)
{
	if (cmd < BM_MAX_VIRTUAL_BUTTONS && mouse_button < BM_MAX_MOUSE_BUTTONS) {
		inputs->buttons[cmd].state = 0;
		inputs->buttons[cmd].mouse_button =
			&inputs->mouse.buttons[mouse_button];
		logger(LOG_INFO,  "Command %s bound to mouse button %d\n",
		       cmd_get_name(cmd), mouse_button);
		return true;
	}

	return false;
}

bool inp_bind_virtual_gamepad_button(input_state_t* inputs, command_t cmd,
				     u32 gamepad, gamepad_button_kind_t button)
{
	if (cmd < BM_MAX_VIRTUAL_BUTTONS && button < BM_MAX_GAMEPAD_BUTTONS) {
		inputs->buttons[cmd].state = 0;
		inputs->buttons[cmd].gamepad_button =
			&inputs->gamepads[gamepad].buttons[button];

		logger(LOG_INFO,
		       "Command %s bound to gamepad %d/button %d (%s)\n",
		       cmd_get_name(cmd), gamepad, button,
		       inputs->gamepads[gamepad].buttons[button].name);

		return true;
	}

	return false;
}
