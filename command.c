#include "bitwise.h"
#include "command.h"

#include <stdlib.h>
#include <string.h>

void cmd_init() {
	array_cmds = (uint32_t *)malloc(sizeof(uint32_t));
	if (array_cmds != NULL) { memset(array_cmds, 0, sizeof(uint32_t)); }
	*array_cmds = 0;
}

bool cmd_getstate(uint32_t cmd)
{
	bool was_cmd_triggered = (bit_check_uint32(*array_cmds, cmd) > 0) ? true : false;

	return was_cmd_triggered;
}

void cmd_shutdown() {
	if (array_cmds) {
		free(array_cmds);
		array_cmds = NULL;
	}
}