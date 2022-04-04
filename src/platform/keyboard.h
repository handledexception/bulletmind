#pragma once

#include "platform/keyboard-scancode.h"

#define MAX_KEYBOARD_KEYS SCANCODE_MAX

enum key_state {
	KEY_UP = 0,
	KEY_DOWN = 1,
};

struct keyboard_key {
	keyboard_scancode_t scancode; /* keyboard scancode */
	u8 state;                     /* key pressed or released state */
};

typedef struct {
	struct keyboard_key key; /* the key */
	keyboard_key_mod_t mod;  /* key modifier */
} keyboard_event_t;
