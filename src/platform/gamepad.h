#pragma once

#include "core/types.h"

#define MAX_GAMEPADS 8

typedef enum {
	GAMEPAD_AXIS_NONE = -1,
	GAMEPAD_AXIS_LEFT_STICK_X = 0,
	GAMEPAD_AXIS_LEFT_STICK_Y = 1,
	GAMEPAD_AXIS_RIGHT_STICK_X = 2,
	GAMEPAD_AXIS_RIGHT_STICK_Y = 3,
	GAMEPAD_AXIS_LEFT_TRIGGER = 4,
	GAMEPAD_AXIS_RIGHT_TRIGGER = 5,
	GAMEPAD_AXES_MAX = 6
} gamepad_axis_kind_t;

typedef enum {
	GAMEPAD_BUTTON_NONE = -1,
	GAMEPAD_BUTTON_A = 0,
	GAMEPAD_BUTTON_B = 1,
	GAMEPAD_BUTTON_X = 2,
	GAMEPAD_BUTTON_Y = 3,
	GAMEPAD_BUTTON_BACK = 4,
	GAMEPAD_BUTTON_GUIDE = 5,
	GAMEPAD_BUTTON_START = 6,
	GAMEPAD_BUTTON_LEFT_STICK = 7,
	GAMEPAD_BUTTON_RIGHT_STICK = 8,
	GAMEPAD_BUTTON_LEFT_SHOULDER = 9,
	GAMEPAD_BUTTON_RIGHT_SHOULDER = 10,
	GAMEPAD_BUTTON_DPAD_UP = 11,
	GAMEPAD_BUTTON_DPAD_DOWN = 12,
	GAMEPAD_BUTTON_DPAD_LEFT = 13,
	GAMEPAD_BUTTON_DPAD_RIGHT = 14,
	GAMEPAD_BUTTON_MAX = 15
} gamepad_button_kind_t;

struct gamepad_axis {
	s32 index;
	const char* name;
	s16 value;
	u64 timestamp;
	gamepad_axis_kind_t kind;
};

struct gamepad_button {
	s32 index;
	const char* name;
	const char* alt_name;
	gamepad_button_kind_t kind;
	u8 state;
	u64 timestamp;
};

struct gamepad {
	const char* name;
	void* instance;
	s32 index;
	bool is_connected;
	u16 product_id;
	u16 vendor_id;
	u16 version;
	struct gamepad_button buttons[GAMEPAD_BUTTON_MAX];
	struct gamepad_axis axes[GAMEPAD_AXES_MAX];
};
