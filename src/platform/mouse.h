#pragma once

#include "core/types.h"
#include "math/vec2.h"
#include <math.h>

#define MAX_MOUSE_BUTTONS 16

typedef struct gui_window gui_window_t;

enum mouse_button_state { MOUSE_BUTTON_DOWN = 1, MOUSE_BUTTON_UP = 0 };

enum mouse_mode { MOUSE_MODE_NORMAL, MOUSE_MODE_RELATIVE };
enum mouse_button {
	MOUSE_BUTTON_LEFT = 0,
	MOUSE_BUTTON_RIGHT = 1,
	MOUSE_BUTTON_MIDDLE = 2,
	MOUSE_BUTTON_X1 = 3,
	MOUSE_BUTTON_X2 = 4,
	MOUSE_BUTTON_NONE,
};

typedef struct {
	u16 button;
	u8 state;
} mouse_button_t;

typedef struct mouse_s {
	enum mouse_mode mode;
	vec2i_t screen_pos;
	vec2i_t screen_pos_last;
	vec2i_t screen_delta;
	vec2i_t window_pos;
	vec2i_t window_pos_last;
	vec2i_t window_delta;
	vec2i_t relative;
	vec2i_t wheel;
	vec2i_t scale_accum;
	f32 sensitivity;
	f32 sensitivity_relative;
	mouse_button_t buttons[MAX_MOUSE_BUTTONS];
	gui_window_t* window;
	bool is_captured;
} mouse_t;

typedef struct {
	gui_window_t* window;	/* window containing mouse cursor */
	mouse_t mouse;			/* mouse state during the event */
	mouse_button_t button;	/*  the mouse button pressed in the event */
} mouse_event_t;

static inline void mouse_buttons_init(mouse_t* mouse)
{
	if (mouse) {
		for (u16 i = 0; i < MAX_MOUSE_BUTTONS; i++) {
			mouse->buttons[i].button = i;
			mouse->buttons[i].state = 0;
		}
		// named mouse buttons
		mouse->buttons[MOUSE_BUTTON_LEFT].button = MOUSE_BUTTON_LEFT;
		mouse->buttons[MOUSE_BUTTON_LEFT].state = false;
		mouse->buttons[MOUSE_BUTTON_MIDDLE].button = MOUSE_BUTTON_MIDDLE;
		mouse->buttons[MOUSE_BUTTON_MIDDLE].state = false;
		mouse->buttons[MOUSE_BUTTON_RIGHT].button = MOUSE_BUTTON_RIGHT;
		mouse->buttons[MOUSE_BUTTON_RIGHT].state = false;
		mouse->buttons[MOUSE_BUTTON_X1].button = MOUSE_BUTTON_X1;
		mouse->buttons[MOUSE_BUTTON_X1].state = false;
		mouse->buttons[MOUSE_BUTTON_X2].button = MOUSE_BUTTON_X2;
		mouse->buttons[MOUSE_BUTTON_X2].state = false;
	}
}

static inline void mouse_init(mouse_t* mouse)
{
	if (mouse) {
		mouse->window = NULL;
		mouse->screen_delta = vec2i_zero();
		mouse->screen_pos = vec2i_zero();
		mouse->window_delta = vec2i_zero();
		mouse->window_pos = vec2i_zero();
		mouse->wheel = vec2i_zero();
		mouse->sensitivity = 1.0f;
		mouse->sensitivity_relative = 1.0f;
		mouse_buttons_init(mouse);
	}
}

static int get_scaled_mouse_delta(float scale, int value, float *accum)
{
    if (scale != 1.0f) {
        *accum += scale * value;
        if (*accum >= 0.0f) {
            value = (int)floor(*accum);
        } else {
            value = (int)ceil(*accum);
        }
        *accum -= value;
    }
    return value;
}

static void mouse_set_button_state(mouse_t* mouse, u16 button, u8 state)
{
	if (mouse) {
		mouse->buttons[button].button = button;
		if (mouse->buttons[button].state != state) {
			mouse->buttons[button].state = state;
		}
	}
}

static u8 mouse_get_button_state(mouse_t* mouse, u16 button)
{
	u8 state = 0;
	if (mouse && mouse->buttons[button].button == button)
		state = mouse->buttons[button].state;
	return state;
}
