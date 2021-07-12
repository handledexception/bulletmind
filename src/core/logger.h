#pragma once

#include <stdarg.h>

enum LOG_LEVEL {
    LOG_ERROR = 100,
    LOG_WARNING = 200,
    LOG_INFO = 300,
    LOG_DEBUG = 400
};

typedef void (*log_handler_t)(enum LOG_LEVEL level, const char* fmt, va_list args, void* param);

void get_log_handler(log_handler_t* handler, void** params);
void set_log_handler(log_handler_t* handler, void* param);

void log_va(enum LOG_LEVEL level, const char* fmt, va_list args);

void logger(enum LOG_LEVEL level, const char* fmt, ...);
