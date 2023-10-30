#pragma once

#include "core/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSTR_NULL '\0'
#define WSTR_NULL L'\0'
#define SSO_SIZE 24 // small-string-optimization
struct string {
	union {
		struct {
			char* data;
			u32* refs;
		};
		struct {
			char data_sso[SSO_SIZE];
		};
	};
	size_t capacity;
	size_t size;
};

typedef struct string string_t;
string_t* str_new(size_t size);
void str_free(string_t* s);
void str_clear(string_t* s);
void str_init(string_t* s);
void str_reserve(string_t* s, size_t capacity);
void str_resize(string_t* s, size_t size);
void str_copy(string_t* dst, const string_t* src);
void str_copy_cstr(string_t* dst, const char* src, size_t len);
const char* str_get(string_t* s);
u32 str_length(string_t* s);
bool str_starts_with(const string_t* s, const char* prefix);
bool str_ends_with(const string_t* s, const char* suffix);
bool str_contains(const string_t* s, const char* substring);

bool cstr_contains(const char* str, const char* substring);
char* cstr_upper_no_copy(char* str, size_t len);
char* cstr_lower_no_copy(char* str, size_t len);
int cstr_first_index_of(const char* str, size_t len, const char c);
int cstr_last_index_of(const char* str, size_t len, const char c);
int astrcmp_n(const char* str1, const char* str2, size_t n);
int wstrcmp_n(const wchar_t* str1, const wchar_t* str2, size_t n);

#ifdef __cplusplus
}
#endif
