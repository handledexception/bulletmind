#pragma once

#include "core/types.h"

struct string {
    char* data;
    size_t length;
    size_t capacity;
};

static inline void str_init(struct string* s);
static inline void str_free(struct string* s);
static inline void str_move(struct string* dst, struct string* src);
