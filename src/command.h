#ifndef _H_COMMAND
#define _H_COMMAND

#include "c99defs.h"
#include "types.h"
#include "engine.h"

u32* array_cmds; // this doesn't need to be a pointer

void cmd_init(void);
bool cmd_getstate(u32 cmd);
void cmd_toggle_bool(u32 cmd, bool* value);
void cmd_refresh(engine_t* engine);
void cmd_shutdown(void);

#define COMMAND_VALS                                                  \
	CMD(CMD_PLAYER_UP, 0x01, "Player Move Up")                    \
	CMD(CMD_PLAYER_DOWN, 0x02, "Player Move Down")                \
	CMD(CMD_PLAYER_LEFT, 0x04, "Player Move Left")                \
	CMD(CMD_PLAYER_RIGHT, 0x08, "Player Move Right")              \
	CMD(CMD_PLAYER_SPEED, 0x10, "Player Speedup")                 \
	CMD(CMD_PLAYER_PRIMARY_FIRE, 0x20, "Player Primary Fire")     \
	CMD(CMD_PLAYER_ALTERNATE_FIRE, 0x40, "Player Alternate Fire") \
	CMD(CMD_SET_FPS_60, 0x80, "Set FPS 60")                       \
	CMD(CMD_SET_FPS_10, 0x100, "Set FPS 10")                      \
	CMD(CMD_QUIT, 0x200, "Quit Game")                             \
	CMD(CMD_SET_DEBUG, 0x400, "Set Debug Mode")

#define CMD(c1, c2, c3) c1 = c2,
enum { COMMAND_VALS };
#undef CMD

#define CMD(c1, c2, c3) c1,
static const u32 COMMAND_LIST[] = {COMMAND_VALS};
#undef CMD

#define CMD(c1, c2, c3) c3,
static const char* COMMAND_NAMES[] = {COMMAND_VALS};
#undef CMD

static volatile const u32 COMMAND_COUNT = sizeof(COMMAND_LIST);

#endif