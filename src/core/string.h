#pragma once

#include "core/types.h"

#ifdef __cplusplus
extern "C" {
#endif
struct string {
	char* data;
	size_t length;
	size_t capacity;
};

// static inline void str_init(struct string* s);
// static inline void str_free(struct string* s);
// static inline void str_move(struct string* dst, struct string* src);
bool str_contains(const char* str, const char* substring);
char* str_upper_no_copy(char* str, size_t len);
char* str_lower_no_copy(char* str, size_t len);
int str_first_index_of(const char* str, size_t len, const char c);
int str_last_index_of(const char* str, size_t len, const char c);
int astrcmp_n(const char* str1, const char* str2, size_t n);
int wstrcmp_n(const wchar_t* str1, const wchar_t* str2, size_t n);

#ifdef __cplusplus
}
#endif
