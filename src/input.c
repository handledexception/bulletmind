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

	inp_set_key_bind(&inputs->key[0], SDL_SCANCODE_ESCAPE, kCommandQuit);
	inp_set_key_bind(&inputs->key[0], SDL_SCANCODE_W, kCommandPlayerUp);
	inp_set_key_bind(&inputs->key[0], SDL_SCANCODE_S, kCommandPlayerDown);
	inp_set_key_bind(&inputs->key[0], SDL_SCANCODE_A, kCommandPlayerLeft);
	inp_set_key_bind(&inputs->key[0], SDL_SCANCODE_D, kCommandPlayerRight);
	inp_set_key_bind(&inputs->key[0], SDL_SCANCODE_LSHIFT, kCommandPlayerSpeed);
	inp_set_key_bind(&inputs->key[0], SDL_SCANCODE_F5, kCommandSetFpsHigh);
	inp_set_key_bind(&inputs->key[0], SDL_SCANCODE_F6, kCommandSetFpsLow);
	inp_set_key_bind(&inputs->key[0], SDL_SCANCODE_F1, kCommandDebugMode);
	//inp_set_key_bind(SDL_SCANCODE_X, 0x7f); // fail case for testing

	inp_set_mouse_button_bind(&inputs->mouse, SDL_BUTTON_LEFT, kCommandPlayerPrimaryFire);
	// inp_set_mouse_button_bind(&inputs->mouse, SDL_BUTTON_MIDDLE, kCommandPlayerAltFire);
	inp_set_mouse_button_bind(&inputs->mouse, SDL_BUTTON_RIGHT, kCommandPlayerAltFire);

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
			inp_set_key_state(&inputs->key[0], evt->key.keysym.scancode, KEY_DOWN);
			break;
		case SDL_KEYUP:
			inp_set_key_state(&inputs->key[0], evt->key.keysym.scancode, KEY_UP);
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
	return true;
}

bool inp_init_mouse(input_state_t* mouse)
{
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

		inputs->gamepad[gdx] = gamepad;

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

		const command_t cmd = keys[scancode].cmd;

		const bool cmd_active = (keys[scancode].state > 0);

		if (cmd < kCommandMax)
			kActiveCommands[cmd] = cmd_active;
	}
}

u8 inp_get_key_state(key_t* keys, u16 scancode)
{
	u8 state = 0;
	if (keys)
		state = keys[scancode].state;
	return state;
}

bool inp_set_key_bind(key_t* keys, u16 scancode, command_t cmd)
{
	bool found_cmd = false;
	const char* cmd_name = NULL;

	cmd_name = cmd_type_to_string(cmd);
	if (cmd < kCommandMax)
		found_cmd = true;

	if (!found_cmd) {
		printf("inp_set_key_bind - error binding Key \"%s\", unknown Command ID \"%d\"!\n",
		       SDL_GetScancodeName(scancode), cmd);
		return false;
	}

	if (keys) {
		key_t* key = &keys[scancode];
		key->cmd = cmd;
		key->scancode = scancode;
		//printf("scancode = %d\n", key);
		printf("inp_set_key_bind - successfully bound Key \"%s\" to Command \"%s\"\n",
		       SDL_GetScancodeName(scancode), cmd_name);
	}

	return true;
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

		const command_t cmd = mouse->buttons[button].cmd;

		const bool cmd_active = (mouse->buttons[button].state > 0);

		if (cmd < kCommandMax)
			kActiveCommands[cmd] = cmd_active;

		printf("inp_set_mouse_state - [button:state:cmd] %d : %d : %d\n", button, state, cmd);
	}
}

u8 inp_get_mouse_button_state(mouse_t* mouse, u16 button)
{
	u8 state = 0;
	if (mouse && mouse->buttons[button].button == button)
		state = mouse->buttons[button].state;
	return state;
}

bool inp_set_mouse_button_bind(mouse_t* mouse, u8 button, command_t cmd)
{
	bool found_cmd = false;
	const char* cmd_name = NULL;

	cmd_name = cmd_type_to_string(cmd);
	if (cmd < kCommandMax)
		found_cmd = true;

	if (!found_cmd) {
		printf("inp_set_mouse_bind - error binding Button \"%d\", unknown Command ID \"%d\"!\n",
		       button, cmd);
		return false;
	}

	if (mouse) {
		mbutton_t* mbutton = &mouse->buttons[button];
		mbutton->cmd = cmd;
		mbutton->button = button;
		printf("inp_set_mouse_bind - successfully bound Button \"%d\" to Command \"%s\"\n",
		       button, cmd_name);
	}

	return true;
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
