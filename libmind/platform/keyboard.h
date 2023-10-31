#pragma once

#include "platform/keyboard-scancode.h"

#define MAX_KEYBOARD_KEYS SCANCODE_MAX

enum key_state {
	KEY_UP = 0,
	KEY_DOWN = 1,
};

typedef struct {
	keyboard_scancode_t scancode; /* keyboard scancode */
	u8 state;                     /* key pressed or released state */
} keyboard_key_t;

typedef struct {
	keyboard_key_t key;     /* the key */
	keyboard_key_mod_t mod; /* key modifier */
} keyboard_event_t;

static void keyboard_init(keyboard_key_t* keys)
{
	if (keys) {
		for (size_t i = SCANCODE_NONE; i < SCANCODE_MAX; i++) {
			keys[i].scancode = (u16)i;
			keys[i].state = 0;
		}
	}
}
