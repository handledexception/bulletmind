#ifndef H_INPUT
#define H_INPUT

#include "c99defs.h"
#include <stdlib.h>

#define KEY_DOWN 1
#define KEY_UP 0

void in_init();
uint32_t in_refresh();
void in_shutdown();

void in_setkeystate(uint16_t key, uint8_t state);
uint8_t in_getkeystate(uint16_t key);
bool in_setkeybind(uint16_t key, int32_t cmd);

bool in_setmbuttonbind(uint8_t button, int32_t cmd);
void in_setmousebuttonstate(uint8_t button, uint8_t state);

#endif
