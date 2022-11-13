/*
 * Copyright (c) 2021 Paul Hindt
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "core/logger.h"
#include "core/video.h"

#include "command.h"
#include "engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

entity_t* entities;

const char* cmd_get_name(command_t cmd)
{
	static char buffer[256];

	switch (cmd) {
	case kCommandMoveForward:
		strcpy(&buffer[0], "Player Move Up");
		break;
	case kCommandMoveBack:
		strcpy(&buffer[0], "Player Move Down");
		break;
	case kCommandMoveLeft:
		strcpy(&buffer[0], "Player Move Left");
		break;
	case kCommandMoveRight:
		strcpy(&buffer[0], "Player Move Right");
		break;
	case kCommandPlayerSpeed:
		strcpy(&buffer[0], "Player Speed");
		break;
	case kCommandPlayerPrimaryFire:
		strcpy(&buffer[0], "Player Primary Fire");
		break;
	case kCommandPlayerAltFire:
		strcpy(&buffer[0], "Player Alternate Fire");
		break;
	case kCommandSetFpsHigh:
		strcpy(&buffer[0], "High FPS");
		break;
	case kCommandSetFpsLow:
		strcpy(&buffer[0], "Low FPS");
		break;
	case kCommandQuit:
		strcpy(&buffer[0], "Quit Game");
		break;
	case kCommandDebugMode:
		strcpy(&buffer[0], "Toggle Debug Mode");
		break;
	case kCommandConsole:
		strcpy(&buffer[0], "Toggle Console");
		break;
	default:
	case kCommandNone:
		break;
	}

	return buffer;
}
