#pragma once

#include "core/types.h"
#include "math/vec2.h"

#define MAX_MOUSE_BUTTONS 16

typedef struct gui_window gui_window_t;

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
    struct vec2i screen_pos;
    struct vec2i window_pos;
    struct vec2i wheel;
    gui_window_t* window;
    struct mouse_button buttons[MAX_MOUSE_BUTTONS];
};

typedef struct {
    struct mouse_button buttons[MAX_MOUSE_BUTTONS]; /* mouse button array */
    struct vec2i screen_pos;                       /* mouse position on virtual screen */
    struct vec2i window_pos;                       /* mouse position in foreground window */
    struct vec2i wheel;                            /* mouse wheel position */
} mouse_event_t;
