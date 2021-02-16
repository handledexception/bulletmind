#pragma once

#include "c99defs.h"
#include "types.h"
#include "engine.h"

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

bool kActiveCommands[kCommandMax];

void cmd_init(void);
bool cmd_getstate(const command_t cmd);
void cmd_toggle_bool(const command_t cmd, bool* value);
void cmd_refresh(engine_t* engine);
void cmd_shutdown(void);
const char* cmd_type_to_string(const command_t cmd);
