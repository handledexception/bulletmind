#include "bitwise.h"
#include "command.h"

#include <stdlib.h>
#include <string.h>

void cmd_init() {
	g_array_cmds = (uint32_t *)malloc(sizeof(uint32_t));
	memset(g_array_cmds, 0, sizeof(uint32_t));
	*g_array_cmds = 0;
}

bool cmd_getstate(uint32_t cmd)
{
	bool was_cmd_triggered = (bit_check_uint32(*g_array_cmds, cmd) > 0) ? true : false;

	return was_cmd_triggered;
}

void cmd_shutdown() {
	if (g_array_cmds) {
		free(g_array_cmds);
		g_array_cmds = NULL;
	}
}