#pragma once

#include "core/types.h"
#include "math/vec2.h"

#define MAX_MOUSE_BUTTONS 16

typedef struct gui_window gui_window_t;

enum mouse_button_state { MOUSE_BUTTON_DOWN = 1, MOUSE_BUTTON_UP = 0 };

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
	gui_window_t* window;
	vec2i_t screen_pos;
	vec2i_t window_pos;
	vec2i_t wheel;
	mouse_button_t buttons[MAX_MOUSE_BUTTONS];
} mouse_t;

typedef struct {
	gui_window_t* window; /* window containing mouse cursor */
	vec2i_t screen_pos;   /* mouse position on virtual screen */
	vec2i_t window_pos;   /* mouse position in foreground window */
	vec2i_t wheel;        /* mouse wheel position */
	mouse_button_t button;
} mouse_event_t;
