#ifndef H_UTILS
#define H_UTILS

#include "c99defs.h"

inline const char* file_extension(const char* filename)
{
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot + 1;
}

#endif