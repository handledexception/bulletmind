#include "core/logger.h"
#include "core/console.h"

#include <stdlib.h>

static void* g_log_param = NULL;

static void default_log_handler(enum log_level level, const char* fmt,
				va_list args, void* param)
{
	(void)param;

	char msg[4096];
	vsnprintf(msg, sizeof(msg), fmt, args);
	switch (level) {
	case LOG_DEBUG:
#ifdef BM_DEBUG
		con_print(LOG_COLOR_CYAN, stdout, "debug: %s\n", msg);
#endif
		break;
	case LOG_INFO:
		con_print(LOG_COLOR_DEFAULT, stdout, "info: %s\n", msg);
		break;
	case LOG_WARNING:
		con_print(LOG_COLOR_YELLOW, stdout, "warn: %s\n", msg);
		break;
	case LOG_ERROR:
		con_print(LOG_COLOR_RED, stdout, "error: %s\n", msg);
		break;
	case LOG_PASS:
		con_print(LOG_COLOR_BRIGHTGREEN, stdout, "pass: %s\n", msg);
		break;
	case LOG_FAIL:
		con_print(LOG_COLOR_BRIGHTRED, stdout, "fail: %s\n", msg);
		break;
	}
}

static log_handler_t g_log_handler = default_log_handler;

void log_va(enum log_level level, const char* fmt, va_list args)
{
	g_log_handler(level, fmt, args, g_log_param);
}

void logger(enum log_level level, const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	log_va(level, fmt, args);
	va_end(args);
}
