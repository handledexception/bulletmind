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

uint32_t *array_cmds;
#define CMD_PLAYER_UP 1
#define CMD_PLAYER_DOWN 2
#define CMD_PLAYER_LEFT 3
#define CMD_PLAYER_RIGHT 4
#define CMD_PLAYER_PRIMARY_FIRE 5
#define CMD_PLAYER_SECONDARY_FIRE 6
#define CMD_QUIT 32

/* #define CMD_PLAYER_UP 1 << 0
#define CMD_PLAYER_DOWN 1 << 2
#define CMD_PLAYER_LEFT 1 << 3
#define CMD_PLAYER_RIGHT 1 << 4
#define CMD_PLAYER_PRIMARY_FIRE 1 << 5
#define CMD_PLAYER_SECONDARY_FIRE 1 << 6
#define CMD_QUIT 1 << 7 */
#endif