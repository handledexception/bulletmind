#pragma once

#include "core/types.h"

struct string {
    char* data;
    size_t length;
    size_t capacity;
};

// static inline void str_init(struct string* s);
// static inline void str_free(struct string* s);
// static inline void str_move(struct string* dst, struct string* src);
void str_upper_no_copy(char* s, size_t len);
void str_lower_no_copy(char* s, size_t len);
s32 str_first_index_of(const char* s, size_t len, const char c);

int astrcmp_n(const char *str1, const char *str2, size_t n);
int wstrcmp_n(const wchar_t *str1, const wchar_t *str2, size_t n);
