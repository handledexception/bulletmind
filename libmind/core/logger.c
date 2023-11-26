#include "core/logger.h"
#include "platform/console.h"

#include <stdlib.h>
#include <time.h>

typedef struct {
	char path[BM_MAX_PATH];
	FILE* log_file;
	log_handler_t handler;
} logger_t;

static logger_t* g_logger = NULL;
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
		con_print(LOG_COLOR_RED, stderr, "error: %s\n", msg);
		break;
	case LOG_PASS:
		con_print(LOG_COLOR_BRIGHTGREEN, stdout, "pass: %s\n", msg);
		break;
	case LOG_FAIL:
		// TODO: should fail log use stderr?
		con_print(LOG_COLOR_BRIGHTRED, stdout, "fail: %s\n", msg);
		break;
	case LOG_NOTE:
		con_print(LOG_COLOR_BRIGHTBLUE, stdout, "note: %s\n", msg);
		break;
	}
}

static void file_log_handler(enum log_level level, const char* fmt,
			     va_list args, void* param)
{
	(void)param;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char datetime[20];
	sprintf(datetime, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900,
		tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	char msg[4096];
	vsnprintf(msg, sizeof(msg), fmt, args);

	switch (level) {
	case LOG_DEBUG:
#ifdef BM_DEBUG
		con_print(LOG_COLOR_CYAN, stdout, "%s|DEBUG|%s\n", datetime,
			  msg);
		fprintf(g_logger->log_file, "%s|DEBUG|%s\n", datetime, msg);
#endif
		break;
	case LOG_INFO:
		con_print(LOG_COLOR_DEFAULT, stdout, "%s|INFO|%s\n", datetime,
			  msg);
		fprintf(g_logger->log_file, "%s|INFO|%s\n", datetime, msg);
		break;
	case LOG_WARNING:
		con_print(LOG_COLOR_YELLOW, stdout, "%s|WARN|%s\n", datetime,
			  msg);
		fprintf(g_logger->log_file, "%s|WARN|%s\n", datetime, msg);
		break;
	case LOG_ERROR:
		con_print(LOG_COLOR_RED, stderr, "%s|ERROR|%s\n", datetime,
			  msg);
		fprintf(g_logger->log_file, "%s|ERROR|%s\n", datetime, msg);
		break;
	case LOG_PASS:
		con_print(LOG_COLOR_BRIGHTGREEN, stdout, "%s|PASS|%s\n",
			  datetime, msg);
		fprintf(g_logger->log_file, "%s|PASS|%s\n", datetime, msg);
		break;
	case LOG_FAIL:
		// TODO: should fail log use stderr?
		con_print(LOG_COLOR_BRIGHTRED, stdout, "%s|FAIL|%s\n", datetime,
			  msg);
		fprintf(g_logger->log_file, "%s|FAIL|%s\n", datetime, msg);
		break;
	case LOG_NOTE:
		con_print(LOG_COLOR_BRIGHTBLUE, stdout, "%s|NOTE|%s\n",
			  datetime, msg);
		fprintf(g_logger->log_file, "%s|NOTE|%s\n", datetime, msg);
		break;
	}
}

static log_handler_t g_log_handler = default_log_handler;

void log_va(enum log_level level, const char* fmt, va_list args)
{
	if (g_logger && g_logger->handler) {
		g_logger->handler(level, fmt, args, g_log_param);
	}
}

void logger_init(const char* path)
{
	g_logger = malloc(sizeof(logger_t));
	if (path != NULL) {
		g_logger->handler = file_log_handler;
		g_logger->log_file = fopen(path, "a");
	}
	if (g_logger->log_file == NULL) {
		g_logger->handler = default_log_handler;
	}
}

void logger_shutdown()
{
	if (g_logger) {
		if (g_logger->log_file) {
			fclose(g_logger->log_file);
			g_logger->log_file = NULL;
		}
		free(g_logger);
		g_logger = NULL;
	}
}

void logger(enum log_level level, const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	log_va(level, fmt, args);
	va_end(args);
}
