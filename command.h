#ifndef H_COMMAND
#define H_COMMAND

#include "c99defs.h"

uint32_t *array_cmds;

void cmd_init();
bool cmd_getstate(uint32_t cmd);
void cmd_shutdown();

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