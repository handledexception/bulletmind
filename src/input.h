#ifndef _H_INPUT
#define _H_INPUT

#include "c99defs.h"
#include "command.h"
#include "types.h"
#include <stdlib.h>

#define KEY_DOWN 1
#define KEY_UP 0

void inp_init();
u32 inp_refresh();
void inp_shutdown();

void inp_set_key_state(u16 key, u8 state);
u8   inp_get_key_state(u16 key);
bool inp_set_key_bind(u16 key, command_t cmd);

bool inp_set_mouse_bind(u8 button, command_t cmd);
u8   inp_get_mouse_state(u16 button);
void inp_set_mouse_state(u8 button, u8 state);

#endif
