#pragma once

#include "c99defs.h"
#include "types.h"
#include "engine.h"

typedef struct input_state_s input_state_t;

typedef enum {
	kCommandNone = -1,
	kCommandFirst = 0,
	kCommandPlayerUp = kCommandFirst,
	kCommandPlayerDown = 1,
	kCommandPlayerLeft = 2,
	kCommandPlayerRight = 3,
	kCommandPlayerSpeed = 4,
	kCommandPlayerPrimaryFire = 5,
	kCommandPlayerAltFire = 6,
	kCommandSetFpsHigh = 7,
	kCommandSetFpsLow = 8,
	kCommandQuit = 9,
	kCommandDebugMode = 10,
	kCommandMax = 11
} command_t;

// bool kActiveCommands[kCommandMax];

void cmd_init(void);
bool cmd_get_state(input_state_t* inputs, const command_t cmd);
void cmd_toggle_bool(input_state_t* inputs, const command_t cmd, bool* value);
void cmd_refresh(engine_t* engine);
void cmd_shutdown(void);
const char* cmd_get_name(const command_t cmd);
