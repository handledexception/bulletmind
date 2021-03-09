#pragma once

#include "c99defs.h"
#include "types.h"

bool is_power_of_two(uintptr_t x);
uintptr_t align_forward(uintptr_t ptr, size_t align);
void *aligned_malloc(size_t size, size_t alignment);
void aligned_free(void *ptr);
