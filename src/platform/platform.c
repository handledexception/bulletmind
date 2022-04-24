/*
 * Portions of this code:
 *
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

#include "platform/platform.h"
#include "platform/utf8.h"

#include "core/string.h"
#include "core/time_convert.h"

#include <errno.h>

FILE* os_wfopen(const wchar_t* path, const char* mode)
{
	FILE* file = NULL;

	if (path) {
#ifdef _MSC_VER
		wchar_t* wcs_mode = NULL;

		os_utf8_to_wcs_ptr(mode, 0, &wcs_mode);

		file = _wfopen(path, wcs_mode);

		free(wcs_mode);
#else
		char* mbs_path = NULL;

		os_wcs_to_utf8_ptr(path, 0, &mbs_path);

		file = fopen(mbs_path, mode);

		free(mbs_path);
#endif
	}

	return file;
}

FILE* os_fopen(const char* path, const char* mode)
{
#ifdef _WIN32
	wchar_t* wpath = NULL;
	FILE* file = NULL;

	if (path) {
		os_utf8_to_wcs_ptr(path, 0, &wpath);

		file = os_wfopen(wpath, mode);

		free(wpath);
	}

	return file;
#else
	return path ? fopen(path, mode) : NULL;
#endif
}

s64 os_fgetsize(FILE* file)
{
	s64 cur_offset = os_ftell_s64(file);
	s64 size;
	int errval = 0;

	if (fseek(file, 0, SEEK_END) == -1)
		return -1;

	size = os_ftell_s64(file);
	if (size == -1)
		errval = errno;

	if (os_fseek_s64(file, cur_offset, SEEK_SET) != 0 && errval != 0)
		errno = errval;

	return size;
}

#ifdef _WIN32
int os_stat(const char* file, struct stat* st)
{
	if (file) {
		wchar_t w_file[512];
		size_t size = os_utf8_to_wcs(file, 0, w_file, sizeof(w_file));
		if (size > 0) {
			struct _stat st_w32;
			int ret = _wstat(w_file, &st_w32);
			if (ret == 0) {
				st->st_dev = st_w32.st_dev;
				st->st_ino = st_w32.st_ino;
				st->st_mode = st_w32.st_mode;
				st->st_nlink = st_w32.st_nlink;
				st->st_uid = st_w32.st_uid;
				st->st_gid = st_w32.st_gid;
				st->st_rdev = st_w32.st_rdev;
				st->st_size = st_w32.st_size;
				st->st_atime = st_w32.st_atime;
				st->st_mtime = st_w32.st_mtime;
				st->st_ctime = st_w32.st_ctime;
			}

			return ret;
		}
	}

	return -1;
}
#endif

int os_fseek_s64(FILE* file, s64 offset, int origin)
{
#ifdef _MSC_VER
	return _fseeki64(file, offset, origin);
#else
	return fseeko(file, offset, origin);
#endif
}

s64 os_ftell_s64(FILE* file)
{
#ifdef _MSC_VER
	return _ftelli64(file);
#else
	return ftello(file);
#endif
}

size_t os_fread_utf8(FILE* file, char** pstr)
{
	size_t size = 0;
	size_t len = 0;

	*pstr = NULL;

	fseek(file, 0, SEEK_END);
	size = (size_t)os_ftell_s64(file);

	if (size > 0) {
		char bom[3];
		char* utf8_str;
		off_t offset;

		bom[0] = 0;
		bom[1] = 0;
		bom[2] = 0;

		// remove BOM
		fseek(file, 0, SEEK_SET);
		size_t size_read = fread(bom, 1, 3, file);
		(void)size_read;

		offset = (astrcmp_n(bom, "\xEF\xBB\xBF", 3) == 0) ? 3 : 0;

		size -= offset;
		if (size == 0)
			return 0;

		utf8_str = malloc(size + 1);
		fseek(file, offset, SEEK_SET);

		size = fread(utf8_str, 1, size, file);
		if (size == 0) {
			free(utf8_str);
			return 0;
		}

		utf8_str[size] = 0;

		*pstr = utf8_str;
	}

	return len;
}

char* os_quick_read_utf8_file(const char* path)
{
	FILE* file = os_fopen(path, "rb");
	char* file_string = NULL;

	if (!file)
		return NULL;

	os_fread_utf8(file, &file_string);
	fclose(file);

	return file_string;
}

s64 os_get_file_size(const char* path)
{
	FILE* file = os_fopen(path, "rb");
	if (!file)
		return -1;

	s64 sz = os_fgetsize(file);
	fclose(file);

	return sz;
}

size_t os_utf8_to_wcs(const char* str, size_t len, wchar_t* dst,
		      size_t dst_size)
{
	size_t in_len;
	size_t out_len;

	if (!str)
		return 0;

	in_len = len ? len : strlen(str);
	out_len = dst ? (dst_size - 1) : utf8_to_wchar(str, in_len, NULL, 0, 0);

	if (dst) {
		if (!dst_size)
			return 0;

		if (out_len)
			out_len =
				utf8_to_wchar(str, in_len, dst, out_len + 1, 0);

		dst[out_len] = 0;
	}

	return out_len;
}

size_t os_utf8_to_wcs_ptr(const char* str, size_t len, wchar_t** pstr)
{
	if (str) {
		size_t out_len = os_utf8_to_wcs(str, len, NULL, 0);

		*pstr = malloc((out_len + 1) * sizeof(wchar_t));
		return os_utf8_to_wcs(str, len, *pstr, out_len + 1);
	} else {
		*pstr = NULL;
		return 0;
	}
}

size_t os_wcs_to_utf8(const wchar_t* str, size_t len, char* dst,
		      size_t dst_size)
{
	size_t in_len = 0;
	size_t out_len = 0;

	if (!str)
		return 0;

	in_len = (len != 0) ? len : wcslen(str);
	out_len = dst ? (dst_size - 1) : wchar_to_utf8(str, in_len, NULL, 0, 0);

	if (dst) {
		if (!dst_size)
			return 0;

		if (out_len)
			out_len =
				wchar_to_utf8(str, in_len, dst, out_len + 1, 0);

		dst[out_len] = 0;
	}

	return out_len;
}

size_t os_wcs_to_utf8_ptr(const wchar_t* str, size_t len, char** pstr)
{
	if (str) {
		size_t out_len = os_wcs_to_utf8(str, len, NULL, 0);

		*pstr = malloc((out_len + 1) * sizeof(char));
		return os_wcs_to_utf8(str, len, *pstr, out_len + 1);
	} else {
		*pstr = NULL;
		return 0;
	}
}

f64 os_get_time_sec(void)
{
	return nsec_to_sec_f64(os_get_time_ns());
}

f64 os_get_time_msec(void)
{
	return nsec_to_msec_f64(os_get_time_ns());
}

architecture_t os_get_architecture(void)
{
#if defined(__i386__) || defined(_M_IX86)
	return BM_ARCH_X86;
#elif defined(__amd64__) || defined(_M_AMD64)
	return BM_ARCH_AMD64;
#elif defined(__aarch64__)
	return BM_ARCH_ARM64;
#elif defined(__arm__)
	return BM_ARCH_ARM32;
#elif defined(__ppc64__) || defined(_ARCH_PPC64)
	return BM_ARCH_PPC64;
#elif defined(__ppc__) || defined(_ARCH_PPC) || defined(_M_PPC)
	return BM_ARCH_PPC32;
#endif
	return BM_ARCH_UNKNOWN;
}

const char* os_architecture_to_string(architecture_t arch)
{
	switch (arch) {
	case BM_ARCH_X86: return "x86";
	case BM_ARCH_AMD64: return "amd64";
	case BM_ARCH_ARM32: return "arm32";
	case BM_ARCH_ARM64: return "arm64";
	case BM_ARCH_PPC32: return "PPC32";
	case BM_ARCH_PPC64: return "PPC64";
	case BM_ARCH_UNKNOWN:
	default:
		return "Unknown";
	}
	return "Unknown";
}

compiler_t os_get_compiler(void)
{
#if defined(__clang__)
	return BM_COMPILER_CLANG;
#elif defined(__GNUC__)
	return BM_COMPILER_GCC;
#elif defined(_MSC_VER)
	return BM_COMPILER_MSVC;
#endif
	return BM_COMPILER_UNKNOWN;
}

const char* os_compiler_to_string(compiler_t comp)
{
	switch (comp) {
	case BM_COMPILER_CLANG: return "Clang";
	case BM_COMPILER_GCC: return "GCC";
	case BM_COMPILER_MSVC: return "MSVC";
	case BM_COMPILER_UNKNOWN:
	default:
		return "Unknown";
	}
	return "Unknown";
}

platform_t os_get_platform(void)
{
#if defined(_WIN32)
	return BM_PLATFORM_WINDOWS;
#elif defined(__APPLE__)
	return BM_PLATFORM_DARWIN;
#elif defined(linux) || defined(__linux__)
	return BM_PLATFORM_LINUX;
#endif
	return BM_PLATFORM_UNKNOWN;
}

const char* os_platform_to_string(platform_t p)
{
	switch (p) {
	case BM_PLATFORM_WINDOWS: return "Windows";
	case BM_PLATFORM_DARWIN: return "Darwin";
	case BM_PLATFORM_LINUX: return "Linux";
	case BM_PLATFORM_UNKNOWN:
	default:
		return "Unknown";
	}
	return "Unknown";
}
