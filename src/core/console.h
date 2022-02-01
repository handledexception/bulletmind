#ifndef H_BM_CONSOLE
#define H_BM_CONSOLE

#include <stdarg.h>
#include <stdio.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

enum log_color {
    LOG_COLOR_DEFAULT = 0,
    LOG_COLOR_RED = 1,
    LOG_COLOR_GREEN = 2,
    LOG_COLOR_YELLOW = 4,
    LOG_COLOR_BLUE = 5,
    LOG_COLOR_CYAN = 6,
    LOG_COLOR_BRIGHTRED = 7,
    LOG_COLOR_BRIGHTGREEN = 8,
    LOG_COLOR_BRIGHTYELLOW = 9,
    LOG_COLOR_BRIGHTBLUE = 10,
    LOG_COLOR_BRIGHTCYAN = 11,
    LOG_COLOR_BOLD = 12,
    LOG_COLOR_NONE
};

static inline int con_print(enum log_color color, FILE* stream, const char* fmt, ...)
{
    va_list args;
    char buffer[256];
    int n = 0;

    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    buffer[sizeof(buffer)-1] = '\0';
    if (color == LOG_COLOR_NONE)
        return fprintf(stream, "%s", buffer);
#if defined(_WIN32)
    HANDLE h = 0;
    CONSOLE_SCREEN_BUFFER_INFO info;
    WORD attr = 0;
    h = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(h, &info);
    switch(color) {
        case LOG_COLOR_RED:            attr = FOREGROUND_RED; break;
        case LOG_COLOR_GREEN:          attr = FOREGROUND_GREEN; break;
        case LOG_COLOR_BLUE:           attr = FOREGROUND_BLUE; break;
        case LOG_COLOR_CYAN:           attr = FOREGROUND_BLUE | FOREGROUND_GREEN; break;
        case LOG_COLOR_YELLOW:         attr = FOREGROUND_RED | FOREGROUND_GREEN; break;
        case LOG_COLOR_BRIGHTRED:      attr = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
        case LOG_COLOR_BRIGHTGREEN:    attr = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
        case LOG_COLOR_BRIGHTCYAN:     attr = FOREGROUND_BLUE | FOREGROUND_GREEN |
                                                FOREGROUND_INTENSITY; break;
        case LOG_COLOR_BRIGHTYELLOW:   attr = FOREGROUND_RED | FOREGROUND_GREEN |
                                                FOREGROUND_INTENSITY; break;
        case LOG_COLOR_BOLD:           attr = FOREGROUND_BLUE | FOREGROUND_GREEN |
                                                FOREGROUND_RED | FOREGROUND_INTENSITY; break;
        default:                        attr = 0; break;
    }
    if(attr != 0)
        SetConsoleTextAttribute(h, attr);
    n = fprintf(stream, "%s", buffer);
    SetConsoleTextAttribute(h, info.wAttributes);
    fflush(stream);
    return n;
#elif defined(__APPLE__) || defined(__linux__)
    const char* str = NULL;
    switch(color) {
        case LOG_COLOR_RED:          str = "\033[0;31m"; break;
        case LOG_COLOR_GREEN:        str = "\033[0;32m"; break;
        case LOG_COLOR_YELLOW:       str = "\033[0;33m"; break;
        case LOG_COLOR_BLUE:         str = "\033[0;34m"; break;
        case LOG_COLOR_CYAN:         str = "\033[0;36m"; break;
        case LOG_COLOR_BRIGHTRED:    str = "\033[1;31m"; break;
        case LOG_COLOR_BRIGHTGREEN:  str = "\033[1;32m"; break;
        case LOG_COLOR_BRIGHTYELLOW: str = "\033[1;33m"; break;
        case LOG_COLOR_BRIGHTBLUE:   str = "\033[1;34m"; break;
        case LOG_COLOR_BRIGHTCYAN:   str = "\033[1;36m"; break;
        case LOG_COLOR_BOLD:         str = "\033[1m"; break;
        default:                      str = "\033[0m"; break;
    }
    fprintf(stream, "%s", str);
    n = printf("%s", buffer);
    fprintf(stream, "\033[0m"); // reset color
    fflush(stream);
    return n;
#endif
}

#endif
