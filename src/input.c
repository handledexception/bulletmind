#include "input.h"
#include "memarena.h"
#include "platform/platform.h"

#include <stdio.h>
#include <string.h>

bool inp_init(input_state_t* inputs)
{
	bool result = false;

	if (!inputs)
		return false;

	inp_init_gamepads(inputs);

	inp_init_keyboard(inputs);

	inp_init_mouse(inputs);

	inp_bind_virtual_key(inputs, kCommandQuit, kScancodeEscape);
	inp_bind_virtual_key(inputs, kCommandPlayerUp, kScancodeW);
	inp_bind_virtual_key(inputs, kCommandPlayerDown, kScancodeS);
	inp_bind_virtual_key(inputs, kCommandPlayerLeft, kScancodeA);
	inp_bind_virtual_key(inputs, kCommandPlayerRight, kScancodeD);
	inp_bind_virtual_key(inputs, kCommandPlayerSpeed, kScancodeLeftShift);
	inp_bind_virtual_key(inputs, kCommandSetFpsHigh, kScancodeF5);
	inp_bind_virtual_key(inputs, kCommandSetFpsLow, kScancodeF6);
	inp_bind_virtual_key(inputs, kCommandDebugMode, kScancodeF1);
	inp_bind_virtual_key(inputs, kCommandPlayerPrimaryFire, kScancodeSpace);
	inp_bind_virtual_mouse_button(inputs, kCommandPlayerPrimaryFire, SDL_BUTTON_LEFT);
	inp_bind_virtual_mouse_button(inputs, kCommandPlayerAltFire, SDL_BUTTON_RIGHT);

	printf("inp_init OK\n");

	return result;
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
	mouse_window_pos.x = (i32)fmx;
	mouse_window_pos.y = (i32)fmy;

	vec2i_t mouse_screen_pos;
	SDL_GetGlobalMouseState(&mouse_screen_pos.x, &mouse_screen_pos.y);

	inp_set_mouse_pos(mouse, mouse_screen_pos, mouse_window_pos);
}

void inp_refresh_pressed(input_state_t* inputs, const SDL_Event* evt)
{
	if (evt) {
		switch (evt->type) {
		case SDL_KEYDOWN:
			inp_set_key_state(&inputs->keys[0], evt->key.keysym.scancode, KEY_DOWN);
			break;
		case SDL_KEYUP:
			inp_set_key_state(&inputs->keys[0], evt->key.keysym.scancode, KEY_UP);
			break;
		case SDL_MOUSEBUTTONDOWN:
			//printf("Mouse Button %d DOWN\n", SDL_BUTTON(evt->button.button));
			inp_set_mouse_button_state(&inputs->mouse, evt->button.button, evt->button.state);
			break;
		case SDL_MOUSEBUTTONUP:
			//printf("Mouse Button %d UP\n", SDL_BUTTON(evt->button.button));
			inp_set_mouse_button_state(&inputs->mouse, evt->button.button, evt->button.state);
			break;
		case SDL_CONTROLLERAXISMOTION:
			printf("Controller Axis Motion - Axis: %d | Value: %d\n", evt->caxis.axis, evt->caxis.value);
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			printf("Controller Button Down - Button: %d\n", evt->cbutton.button);
			break;
		case SDL_CONTROLLERBUTTONUP:
			printf("Controller Button Up - Button: %d\n", evt->cbutton.button);
			break;
		case SDL_CONTROLLERDEVICEADDED:
			printf("Controller Device Added\n");
			break;
		case SDL_CONTROLLERDEVICEREMOVED:
			printf("Controller Device Removed\n");
			break;
		case SDL_CONTROLLERDEVICEREMAPPED:
			printf("Controller Device Remapped\n");
			break;
		}
	}
}

void inp_shutdown(input_state_t* inputs)
{
	printf("inp_shutdown OK\n");
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
	inputs->mouse.buttons[SDL_BUTTON_MIDDLE].button = SDL_BUTTON_MIDDLE;
	inputs->mouse.buttons[SDL_BUTTON_RIGHT].button = SDL_BUTTON_RIGHT;
	inputs->mouse.buttons[SDL_BUTTON_X1].button = SDL_BUTTON_X1;
	inputs->mouse.buttons[SDL_BUTTON_X2].button = SDL_BUTTON_X2;
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
				printf("SDL_GameControllerOpen error: %s\n", sdl_err);
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

		gamepad_err = inp_get_gamepad_axes(&gamepad);

		inputs->gamepads[gdx] = gamepad;

		printf("Gamepad %s connected | Product ID: %d | Vendor ID: %d | Version: %d\n",
			gamepad.name, gamepad.product_id, gamepad.vendor_id, gamepad.version);
	}

	return true;
}

void inp_set_key_state(key_t* keys, u16 scancode, u8 state)
{
	if (keys) {
		if (keys[scancode].state != state) {
			keys[scancode].state = state;
		}

		keys[scancode].timestamp = os_get_time_ns();
	}
}

u8 inp_get_key_state(key_t* keys, u16 scancode)
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
		}

		mouse->buttons[button].timestamp = os_get_time_ns();
	}
}

u8 inp_get_mouse_button_state(mouse_t* mouse, u16 button)
{
	u8 state = 0;
	if (mouse && mouse->buttons[button].button == button)
		state = mouse->buttons[button].state;
	return state;
}

gamepad_axis_kind_t inp_gamepad_axis_kind_from_sdl(const SDL_GameControllerAxis axis)
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

bool inp_get_gamepad_axes(gamepad_t* gamepad)
{
	bool axis_err = false;

	for (i32 adx = (i32)kGamepadAxisLeftStickX; adx < (i32)kGamepadAxisMax; adx++) {
		const SDL_GameControllerAxis axis = (SDL_GameControllerAxis)adx;
		const char* axis_name = SDL_GameControllerGetStringForAxis(axis);
		const i16 axis_value = SDL_GameControllerGetAxis((SDL_GameController*)gamepad->instance, axis);

		// SDL_GameControllerGetAxis returns 0 on success or failure. Must use SDL_GetError to check for errors.
		if (axis_value == 0) {
			const char* err = SDL_GetError();
			if (err != NULL && err[0] != '\0') {
				printf("SDL_GameControllerGetAxis error: %s\n", err);
				axis_err = true;
			}
		}

		gamepad->axes[adx].name = axis_name;
		gamepad->axes[adx].value = axis_value;
		gamepad->axes[adx].kind = inp_gamepad_axis_kind_from_sdl(axis);
		gamepad->axes[adx].timestamp = os_get_time_ns();
	}

	return axis_err;
}

bool inp_bind_virtual_key(input_state_t* inputs, command_t cmd, u16 scancode)
{
	if (cmd < kMaxVirtualButtons) {
		inputs->buttons[cmd].state = 0;
		inputs->buttons[cmd].keyboard_key = &inputs->keys[scancode];
		printf("Command %s bound to key %d\n", cmd_get_name(cmd), scancode);
		return true;
	}

	return false;
}

bool inp_bind_virtual_mouse_button(input_state_t* inputs, command_t cmd, u16 mouse_button)
{
	if (cmd < kMaxVirtualButtons) {
		inputs->buttons[cmd].state = 0;
		inputs->buttons[cmd].mouse_button = &inputs->mouse.buttons[mouse_button];
		printf("Command %s bound to mouse button %d\n", cmd_get_name(cmd), mouse_button);
		return true;
	}

	return false;
}

bool inp_bind_virtual_gamepad_button(input_state_t* inputs, command_t cmd, u32 gamepad, u32 gamepad_button)
{
	if (cmd < kMaxVirtualButtons) {
		inputs->buttons[cmd].state = 0;
		inputs->buttons[cmd].gamepad_button = &inputs->gamepads[gamepad].buttons[gamepad_button];
		printf("Command %s bound to gamepad %d/button %d\n", cmd_get_name(cmd), gamepad, gamepad_button);
		return true;
	}

	return false;
}
