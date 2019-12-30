#ifndef H_INPUT
#define H_INPUT

#include "c99defs.h"
#include <stdlib.h>

#define KEY_DOWN 1
#define KEY_UP 0

void inp_init();
uint32_t inp_refresh();
void inp_shutdown();

void inp_set_key_state(uint16_t key, uint8_t state);
uint8_t inp_get_key_state(uint16_t key);
bool inp_set_key_bind(uint16_t key, int32_t cmd);

bool inp_set_mouse_bind(uint8_t button, int32_t cmd);
void inp_set_mouse_state(uint8_t button, uint8_t state);

#endif
