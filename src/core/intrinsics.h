/*
 * Copyright (c) 2022 Paul Hindt
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

#ifndef H_BM_INTRINSICS
#define H_BM_INTRINSICS

#define BM_BUILD_DATE __DATE__
#define BM_BUILD_TIME __TIME__

#if defined(_WIN32)
#ifndef BM_WINDOWS
#define BM_WINDOWS
#endif
#elif defined(__APPLE__)
#ifndef BM_DARWIN
#define BM_DARWIN
#endif
#elif defined(linux) || defined(__linux__)
#ifndef BM_LINUX
#define BM_LINUX
#endif
#endif

#if defined(__i386__) || defined(_M_IX86)
#define BM_ARCH_X86
#elif defined(__amd64__) || defined(_M_AMD64)
#define BM_ARCH_AMD64
#elif defined(__aarch64__)
#define BM_ARCH_ARM64
#elif defined(__arm__)
#define BM_ARCH_ARM32
#elif defined(__ppc64__) || defined(_ARCH_PPC64)
#define BM_ARCH_PPC64
#elif defined(__ppc__) || defined(_ARCH_PPC) || defined(_M_PPC)
#define BM_ARCH_PPC32
#endif

#if defined(BM_ARCH_X86) || defined(BM_ARCH_ARM32) || defined(BM_ARCH_PPC32)
#define BM_BITS_32
#elif defined(BM_ARCH_AMD64) || defined(BM_ARCH_ARM64) || defined(BM_ARCH_PPC64)
#define BM_BITS_64
#endif

#if defined(_MSC_VER)
#define BM_COMPILER_MSVC
#elif defined(__GNUC__)
#define BM_COMPILER_GCC
#elif defined(__clang__)
#define BM_COMPILER_CLANG
#endif

#endif
