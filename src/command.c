#include "command.h"
#include "entity.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

entity_t *entities;

void cmd_init(void) {
    //todo(paulh): this is all nonsense, just make array_cmds a regular uint32
    array_cmds = (uint32_t *)malloc(sizeof(uint32_t));
    if (array_cmds != NULL) { memset(array_cmds, 0, sizeof(uint32_t)); }
    *array_cmds = 0;
    printf("cmd_init OK\n");
}

bool cmd_getstate(uint32_t cmd)
{
    //bool triggeredcmd = (bit_check_uint32(*array_cmds, cmd) > 0) ? true : false;
    bool triggeredcmd = (*array_cmds & cmd);
    //printf("triggered: %d\n", triggeredcmd);
    return triggeredcmd;
}

void cmd_toggle_bool(uint32_t cmd, bool* value)
{
    static bool toggled = false;
    if (cmd_getstate(cmd) == true) {
        if (toggled == false) {
            if (value != NULL) {
                if (*value == true)
                    *value = false;
                else
                    *value = true;
            }
            toggled = true;
        }
    }
    else {
        if (toggled == true)
            toggled = false;
    }
}

void cmd_refresh(engine_t* eng)
{
    // static bool toggled = false;
    if (cmd_getstate(CMD_QUIT) == true) { eng->state = ES_QUIT; }

    cmd_toggle_bool(CMD_SET_DEBUG, &eng->debug);

    if (cmd_getstate(CMD_SET_FPS_60) == true) { eng->target_frametime = TARGET_FRAMETIME(60); }
    if (cmd_getstate(CMD_SET_FPS_10) == true) { eng->target_frametime = TARGET_FRAMETIME(10); }
}

void cmd_shutdown(void) {
    if (array_cmds) {
        free(array_cmds);
        array_cmds = NULL;
    }

    printf("cmd_shutdown OK\n");
}