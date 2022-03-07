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

// #ifndef log_warn
// 	#define log_warn(fmt, ...) \
// 		do { logger(LOG_WARNING, fmt, __VA_ARGS__); } while(0)
// #endif
// #ifndef log_error
// 	#define logger_error(LOG_ERROR,  fmt, ...) \
// 		do { logger(LOG_ERROR, fmt, __VA_ARGS__); } while(0)
// #endif
// #ifndef log_info
// 	#define log_info(LOG_INFO,  fmt, ...) \
// 		do {  logger(LOG_INFO, fmt, __VA_ARGS__); } while(0)
// #endif
// #ifndef log_debug
// 	#define logger(LOG_DEBUG,  fmt, ...) \
// 		do { logger(LOG_DEBUG, fmt, __VA_ARGS__); } while(0)
// #endif
// #ifndef log_pass
// 	#define log_pass(fmt, ...) \
// 		do {  logger(LOG_PASS, fmt, __VA_ARGS__); } while(0)
// #endif
// #ifndef log_fail
// 	#define log_fail(fmt, ...) \
// 		do {  logger(LOG_FAIL, fmt, __VA_ARGS__); } while(0)
// #endif
