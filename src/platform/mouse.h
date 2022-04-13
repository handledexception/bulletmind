#pragma once

#include "core/types.h"
#include "math/vec2.h"

#define MAX_MOUSE_BUTTONS 16

typedef struct gui_window gui_window_t;

enum mouse_button_state {
	MOUSE_BUTTON_DOWN = 1,
	MOUSE_BUTTON_UP = 0
};

enum _mouse_button {
	MOUSE_BUTTON_LEFT = 0,
	MOUSE_BUTTON_RIGHT = 1,
	MOUSE_BUTTON_MIDDLE = 2,
	MOUSE_BUTTON_X1 = 3,
	MOUSE_BUTTON_X2 = 4,
};

struct mouse_button {
	u16 button;
	u8 state;
};

struct mouse_device {
	gui_window_t* window;
	struct vec2i screen_pos;
	struct vec2i window_pos;
	struct vec2i wheel;
	struct mouse_button buttons[MAX_MOUSE_BUTTONS];
};

typedef struct {
	gui_window_t* window;    /* window containing mouse cursor */
	struct vec2i screen_pos; /* mouse position on virtual screen */
	struct vec2i window_pos; /* mouse position in foreground window */
	struct vec2i wheel;      /* mouse wheel position */
	struct mouse_button buttons[MAX_MOUSE_BUTTONS]; /* mouse button array */
} mouse_event_t;
