#pragma once

#include <stdarg.h>

enum log_level {
	LOG_ERROR = 100,
	LOG_WARNING = 200,
	LOG_INFO = 300,
	LOG_DEBUG = 400,
	LOG_PASS = 500,
	LOG_FAIL = 600,
	LOG_NOTE = 700,
};

typedef void (*log_handler_t)(enum log_level level, const char* fmt,
			      va_list args, void* param);

void get_log_handler(log_handler_t* handler, void** params);
void set_log_handler(log_handler_t* handler, void* param);
void log_va(enum log_level level, const char* fmt, va_list args);
void logger(enum log_level level, const char* fmt, ...);
