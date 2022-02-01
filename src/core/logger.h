#pragma once

#include <stdarg.h>

enum log_level {
	LOG_ERROR = 100,
	LOG_WARNING = 200,
	LOG_INFO = 300,
	LOG_DEBUG = 400,
	LOG_PASS = 500,
	LOG_FAIL = 600,
};

typedef void (*log_handler_t)(enum log_level level, const char* fmt,
			      va_list args, void* param);

void get_log_handler(log_handler_t* handler, void** params);
void set_log_handler(log_handler_t* handler, void* param);
void log_va(enum log_level level, const char* fmt, va_list args);
void logger(enum log_level level, const char* fmt, ...);

#define warn(fmt, ...) logger(LOG_WARNING, fmt, __VA_ARGS__)
#define error(fmt, ...) logger(LOG_ERROR, fmt, __VA_ARGS__)
#define info(fmt, ...) logger(LOG_INFO, fmt, __VA_ARGS__)
#define debug(fmt, ...) logger(LOG_DEBUG, fmt, __VA_ARGS__)
#define pass(fmt, ...) logger(LOG_PASS, fmt, __VA_ARGS__)
#define fail(fmt, ...) logger(LOG_FAIL, fmt, __VA_ARGS__)
