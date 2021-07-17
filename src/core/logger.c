#include "core/logger.h"

#include <stdio.h>
#include <stdlib.h>

static void* g_log_param = NULL;

static void default_log_handler(enum LOG_LEVEL level, const char* fmt,
				va_list args, void* param)
{
	(void)param;

	char msg[4096];
	vsnprintf(msg, sizeof(msg), fmt, args);
	switch (level) {
	case LOG_DEBUG:
		fprintf(stdout, "debug: %s\n", msg);
		fflush(stdout);
		break;
	case LOG_INFO:
		fprintf(stdout, "info: %s\n", msg);
		fflush(stdout);
		break;
	case LOG_WARNING:
		fprintf(stdout, "warning: %s\n", msg);
		fflush(stdout);
		break;
	case LOG_ERROR:
		fprintf(stdout, "error: %s\n", msg);
		fflush(stdout);
		break;
	}
}

static log_handler_t g_log_handler = default_log_handler;

void log_va(enum LOG_LEVEL level, const char* fmt, va_list args)
{
	g_log_handler(level, fmt, args, g_log_param);
}

void logger(enum LOG_LEVEL level, const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	log_va(level, fmt, args);
	va_end(args);
}
