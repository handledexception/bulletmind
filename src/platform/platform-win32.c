/*
 * Copyright (c) 2013 Hugh Bailey <obs.jim@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <Windows.h>

#include "platform/platform.h"
// #include "platform/win-version.h"
#include "core/logger.h"

static LARGE_INTEGER clock_freq;
static bool clock_initialized = false;

static inline u64 get_clock_freq(void)
{
	if (!clock_initialized) {
		QueryPerformanceFrequency(&clock_freq);
		clock_initialized = true;
	}
	return clock_freq.QuadPart;
}

void os_sleep_ms(const u32 duration)
{
	u32 d = duration;
	// if (get_win_ver() >= 0x0602 & duration > 0)
	//     d--;
	Sleep(d);
}

u64 os_get_time_ns(void)
{
	LARGE_INTEGER current_time;
	f64 time_val = 0.0;

	QueryPerformanceCounter(&current_time);
	time_val = (f64)current_time.QuadPart;
	time_val *= 1000000000.0;
	time_val /= (f64)get_clock_freq();

	return (u64)time_val;
}

bool os_file_exists(const char* path)
{
	WIN32_FIND_DATAW wfd;
	HANDLE hFind;
	wchar_t* path_utf16;

	if (!os_utf8_to_wcs_ptr(path, 0, &path_utf16))
		return false;

	hFind = FindFirstFileW(path_utf16, &wfd);
	if (hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);

	free(path_utf16);
	return hFind != INVALID_HANDLE_VALUE;
}

void* os_dlopen(const char* path)
{
	if (!path)
		return NULL;

	wchar_t* path_wide;
	wchar_t* path_sep_wide;
	
	HMODULE module = NULL;
	
	os_utf8_to_wcs_ptr(path, 0, &path_wide);

	module = LoadLibraryW(path_wide);
	if (!module) {
		logger(LOG_INFO, "LoadLibrary error %s", path);
		return NULL;
	}
	
	return module;
}

void* os_dlsym(void* module, const char* func)
{
	return GetProcAddress(module, func);
}

void os_dlclose(void* module)
{
	FreeLibrary(module);
}
