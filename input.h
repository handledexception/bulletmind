#ifndef H_INPUT
#define H_INPUT

#include "c99defs.h"
#include <stdlib.h>

void 		in_init();
uint32_t	in_refresh();
void		in_shutdown();

void 		in_setkeystate(uint16_t key, uint8_t state);
uint8_t 	in_getkeystate(uint16_t key);
void		in_setkeybind(uint16_t key, int32_t cmd);

#endif