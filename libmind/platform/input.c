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

#include "platform/input.h"
#include "core/logger.h"
#include "core/memory.h"
#include "platform/platform.h"
#include "gui/gui.h"

#include <stdio.h>
#include <string.h>

struct input_state* inp_new()
{
	struct input_state* inputs = BM_ALLOC(sizeof(struct input_state));
	inp_init(inputs);
	return inputs;
}

void inp_free(struct input_state* inputs)
{
	if (inputs != NULL) {
		BM_FREE(inputs);
		inputs = NULL;
	}
}

result inp_init(struct input_state* inputs)
{
	if (inputs == NULL)
		return RESULT_NULL;

	memset(inputs, 0, sizeof(*inputs));

	keyboard_init(&inputs->keys[0]);
	mouse_init(&inputs->mouse);

	for (size_t i = 0; i < MAX_VIRTUAL_BUTTONS; i++) {
		inputs->buttons[i].name = NULL;
		inputs->buttons[i].state = 0;
		inputs->buttons[i].mouse_button = NULL;
		inputs->buttons[i].keyboard_key = NULL;
		inputs->buttons[i].pad_button = NULL;
		inputs->buttons[i].toggled = false;
	}

	return RESULT_OK;
}

void inp_refresh_mouse(mouse_t* mouse, f32 scale_x, f32 scale_y)
{
	// int mx = 0;
	// int my = 0;
	// SDL_GetMouseState(&mx, &my);
	// f32 fmx = (f32)mx;
	// f32 fmy = (f32)my;
	// fmx /= scale_x;
	// fmy /= scale_y;

	// vec2i_t mouse_window_pos;
	// mouse_window_pos.x = (s32)fmx;
	// mouse_window_pos.y = (s32)fmy;

	// vec2i_t mouse_screen_pos;
	// SDL_GetGlobalMouseState(&mouse_screen_pos.x, &mouse_screen_pos.y);

	// inp_set_mouse_pos(mouse, mouse_screen_pos, mouse_window_pos);
}

void inp_read_gui_event(struct input_state* inputs, const gui_event_t* evt)
{
	if (evt) {
		switch (evt->type) {
		case GUI_EVENT_KEY_DOWN:
		case GUI_EVENT_KEY_UP:
			inp_set_key_state(&inputs->keys[0],
					  evt->keyboard.key.scancode,
					  evt->keyboard.key.state);
			break;
		case GUI_EVENT_MOUSE_BUTTON_DOWN:
		case GUI_EVENT_MOUSE_BUTTON_UP:
			mouse_set_button_state(&inputs->mouse,
					       evt->mouse.button.button,
					       evt->mouse.button.state);
			break;
			/*
		case SDL_CONTROLLERAXISMOTION:
			for (size_t cdx = 0; cdx < MAX_GAMEPADS; cdx++) {
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
			for (size_t cdx = 0; cdx < MAX_GAMEPADS; cdx++) {
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
			for (size_t cdx = 0; cdx < MAX_GAMEPADS; cdx++) {
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
		*/
		}
	}
}

void inp_shutdown(struct input_state* inputs)
{
	logger(LOG_INFO, "inp_shutdown OK\n");
}

// bool inp_init_gamepads(struct input_state* inputs)
// {
// 	const int num_joysticks = SDL_NumJoysticks();
// 	if (num_joysticks == 0)
// 		return true;

// 	bool gamepad_err = false;
// 	for (int gdx = 0; gdx < num_joysticks; gdx++) {
// 		SDL_GameController* gc = SDL_GameControllerOpen(gdx);
// 		if (gc == NULL) {
// 			const char* sdl_err = SDL_GetError();
// 			if (sdl_err != NULL && sdl_err[0] != '\0')
// 				logger(LOG_ERROR,
// 				       "SDL_GameControllerOpen error: %s\n",
// 				       sdl_err);
// 			gamepad_err = true;
// 			continue;
// 		}

// 		struct gamepad gamepad;
// 		memset(&gamepad, 0, sizeof(struct gamepad));
// 		gamepad.instance = (void*)gc;
// 		gamepad.name = SDL_GameControllerName(gc);
// 		gamepad.index = gdx;
// 		gamepad.is_connected = true;
// 		gamepad.product_id = SDL_GameControllerGetProduct(gc);
// 		gamepad.vendor_id = SDL_GameControllerGetVendor(gc);
// 		gamepad.version = SDL_GameControllerGetProductVersion(gc);

// 		gamepad_err = inp_enumerate_gamepad_axes(&gamepad);
// 		if (!gamepad_err)
// 			gamepad_err = inp_enumerate_gamepad_buttons(&gamepad);

// 		inputs->gamepads[gdx] = gamepad;

// 		logger(LOG_INFO,
// 		       "Gamepad %s connected | Product ID: %d | Vendor ID: %d | Version: %d\n",
// 		       gamepad.name, gamepad.product_id, gamepad.vendor_id,
// 		       gamepad.version);
// 	}

// 	return !gamepad_err;
// }

void inp_set_key_state(keyboard_key_t* keys, u16 scancode, u8 state)
{
	if (keys) {
		if (keys[scancode].state != state) {
			keys[scancode].state = state;
			// keys[scancode].timestamp = os_get_time_ns();
		}
	}
}

u8 inp_get_key_state(keyboard_key_t* keys, u16 scancode)
{
	u8 state = 0;
	if (keys)
		state = keys[scancode].state;
	return state;
}

void inp_update_mouse(struct input_state* inputs, const mouse_t* mouse)
{
	if (inputs && mouse) {
		memcpy(&inputs->mouse, mouse, sizeof(mouse_t));
	}
}
// const char* inp_gamepad_button_kind_to_string(gamepad_button_kind_t kind)
// {
// 	switch (kind) {
// 	default:
// 	case GAMEPAD_BUTTON_NONE:
// 	case GAMEPAD_BUTTON_MAX:
// 		return "None";
// 	case GAMEPAD_BUTTON_A:
// 		return "A";
// 	case GAMEPAD_BUTTON_B:
// 		return "B";
// 	case GAMEPAD_BUTTON_X:
// 		return "X";
// 	case GAMEPAD_BUTTON_Y:
// 		return "Y";
// 	case GAMEPAD_BUTTON_BACK:
// 		return "Back";
// 	case GAMEPAD_BUTTON_GUIDE:
// 		return "Guide";
// 	case GAMEPAD_BUTTON_START:
// 		return "Start";
// 	case GAMEPAD_BUTTON_LEFT_STICK:
// 		return "LS";
// 	case GAMEPAD_BUTTON_RIGHT_STICK:
// 		return "RS";
// 	case GAMEPAD_BUTTON_LEFT_SHOULDER:
// 		return "Right Shoulder";
// 	case GAMEPAD_BUTTON_RIGHT_SHOULDER:
// 		return "Left Shoulder";
// 	case GAMEPAD_BUTTON_DPAD_UP:
// 		return "D-Pad Up";
// 	case GAMEPAD_BUTTON_DPAD_DOWN:
// 		return "D-Pad Down";
// 	case GAMEPAD_BUTTON_DPAD_LEFT:
// 		return "D-Pad Left";
// 	case GAMEPAD_BUTTON_DPAD_RIGHT:
// 		return "D-Pad Right";
// 	}
// }

// gamepad_button_kind_t
// inp_gamepad_button_kind_from_sdl(const SDL_GameControllerButton button)
// {
// 	gamepad_button_kind_t kind = GAMEPAD_BUTTON_NONE;

// 	switch (button) {
// 	case SDL_CONTROLLER_BUTTON_INVALID:
// 		kind = GAMEPAD_BUTTON_NONE;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_A:
// 		kind = GAMEPAD_BUTTON_A;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_B:
// 		kind = GAMEPAD_BUTTON_B;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_X:
// 		kind = GAMEPAD_BUTTON_X;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_Y:
// 		kind = GAMEPAD_BUTTON_Y;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_BACK:
// 		kind = GAMEPAD_BUTTON_BACK;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_GUIDE:
// 		kind = GAMEPAD_BUTTON_GUIDE;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_START:
// 		kind = GAMEPAD_BUTTON_START;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_LEFTSTICK:
// 		kind = GAMEPAD_BUTTON_LEFT_STICK;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
// 		kind = GAMEPAD_BUTTON_RIGHT_STICK;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
// 		kind = GAMEPAD_BUTTON_LEFT_SHOULDER;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
// 		kind = GAMEPAD_BUTTON_RIGHT_SHOULDER;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_DPAD_UP:
// 		kind = GAMEPAD_BUTTON_DPAD_UP;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
// 		kind = GAMEPAD_BUTTON_DPAD_DOWN;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
// 		kind = GAMEPAD_BUTTON_DPAD_LEFT;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
// 		kind = GAMEPAD_BUTTON_DPAD_RIGHT;
// 		break;
// 	case SDL_CONTROLLER_BUTTON_MAX:
// 		kind = GAMEPAD_BUTTON_MAX;
// 		break;
// 	default:
// 		kind = GAMEPAD_BUTTON_NONE;
// 		break;
// 	}

// 	return kind;
// }

// gamepad_axis_kind_t
// inp_gamepad_axis_kind_from_sdl(const SDL_GameControllerAxis axis)
// {
// 	gamepad_axis_kind_t kind = GAMEPAD_AXIS_NONE;

// 	switch (axis) {
// 	case SDL_CONTROLLER_AXIS_INVALID:
// 		kind = GAMEPAD_AXIS_NONE;
// 		break;
// 	case SDL_CONTROLLER_AXIS_LEFTX:
// 		kind = GAMEPAD_AXIS_LEFT_STICK_X;
// 		break;
// 	case SDL_CONTROLLER_AXIS_LEFTY:
// 		kind = GAMEPAD_AXIS_LEFT_STICK_Y;
// 		break;
// 	case SDL_CONTROLLER_AXIS_RIGHTX:
// 		kind = GAMEPAD_AXIS_RIGHT_STICK_X;
// 		break;
// 	case SDL_CONTROLLER_AXIS_RIGHTY:
// 		kind = GAMEPAD_AXIS_RIGHT_STICK_Y;
// 		break;
// 	case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
// 		kind = GAMEPAD_AXIS_LEFT_TRIGGER;
// 		break;
// 	case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
// 		kind = GAMEPAD_AXIS_RIGHT_TRIGGER;
// 		break;
// 	case SDL_CONTROLLER_AXIS_MAX:
// 		kind = GAMEPAD_AXES_MAX;
// 		break;
// 	}

// 	return kind;
// }

// bool inp_enumerate_gamepad_buttons(struct gamepad* gamepad)
// {
// 	bool btn_err = false;

// 	for (s32 bdx = GAMEPAD_BUTTON_A; bdx < GAMEPAD_BUTTON_MAX; bdx++) {
// 		const SDL_GameControllerButton button =
// 			(SDL_GameControllerButton)bdx;

// 		const char* button_name =
// 			SDL_GameControllerGetStringForButton(button);

// 		const u8 button_state = SDL_GameControllerGetButton(
// 			(SDL_GameController*)gamepad->instance, button);

// 		const gamepad_button_kind_t kind =
// 			inp_gamepad_button_kind_from_sdl(button);

// 		gamepad->buttons[kind].index = bdx;
// 		gamepad->buttons[kind].kind = kind;
// 		gamepad->buttons[kind].state = button_state;
// 		gamepad->buttons[kind].name = button_name;
// 		gamepad->buttons[kind].timestamp = os_get_time_ns();

// 		logger(LOG_INFO, "Enumerated gamepad button %s\n", button_name);
// 	}

// 	return btn_err;
// }

// bool inp_enumerate_gamepad_axes(struct gamepad* gamepad)
// {
// 	bool axis_err = false;

// 	for (s32 adx = (s32)GAMEPAD_AXIS_LEFT_STICK_X;
// 	     adx < (s32)GAMEPAD_AXES_MAX; adx++) {
// 		const SDL_GameControllerAxis axis = (SDL_GameControllerAxis)adx;

// 		const char* axis_name =
// 			SDL_GameControllerGetStringForAxis(axis);

// 		const s16 axis_value = SDL_GameControllerGetAxis(
// 			(SDL_GameController*)gamepad->instance, axis);

// 		// SDL_GameControllerGetAxis returns 0 on success or failure. Must use SDL_GetError to check for errors.
// 		if (axis_value == 0) {
// 			const char* err = SDL_GetError();
// 			if (err != NULL && err[0] != '\0') {
// 				logger(LOG_INFO,
// 				       "SDL_GameControllerGetAxis error: %s\n",
// 				       err);
// 				axis_err = true;
// 			}
// 		}

// 		const gamepad_axis_kind_t kind =
// 			inp_gamepad_axis_kind_from_sdl(axis);
// 		gamepad->axes[kind].index = adx;
// 		gamepad->axes[kind].name = axis_name;
// 		gamepad->axes[kind].value = axis_value;
// 		gamepad->axes[kind].kind = kind;
// 		gamepad->axes[kind].timestamp = os_get_time_ns();

// 		logger(LOG_INFO, "Enumerated gamepad axis %s\n", axis_name);
// 	}

// 	return axis_err;
// }

// void inp_set_gamepad_button_state(struct gamepad* gamepad,
// 				  gamepad_button_kind_t button, u8 state)
// {
// 	if (gamepad && gamepad->buttons[button].state != state) {
// 		gamepad->buttons[button].state = state;
// 		gamepad->buttons[button].timestamp = os_get_time_ns();
// 	}
// }

// u8 inp_get_gamepad_button_state(struct gamepad* gamepad,
// 				gamepad_button_kind_t button)
// {
// 	u8 state = 0;
// 	if (gamepad)
// 		state = gamepad->buttons[button].state;
// 	return state;
// }

// void inp_set_gamepad_axis_value(struct gamepad* gamepad,
// 				gamepad_axis_kind_t axis, u16 value)
// {
// 	if (gamepad)
// 		gamepad->axes[axis].value = value;
// }

// s16 inp_get_gamepad_axis_value(struct gamepad* gamepad,
// 			       gamepad_axis_kind_t axis)
// {
// 	s16 value = 0;
// 	if (gamepad)
// 		value = gamepad->axes[axis].value;
// 	return value;
// }