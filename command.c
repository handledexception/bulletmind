//#include "bitwise.h"
#include "command.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cmd_init() {
	array_cmds = (uint32_t *)malloc(sizeof(uint32_t));
	if (array_cmds != NULL) { memset(array_cmds, 0, sizeof(uint32_t)); }
	*array_cmds = 0;
	printf("cmd_init OK\n");
}

bool cmd_getstate(uint32_t cmd)
{
	//bool triggeredcmd = (bit_check_uint32(*array_cmds, cmd) > 0) ? true : false;	
	bool triggeredcmd = (*array_cmds & cmd);
	return triggeredcmd;
}

void cmd_shutdown() {
	if (array_cmds) {
		free(array_cmds);
		array_cmds = NULL;
	}

	printf("cmd_shutdown OK\n");
}