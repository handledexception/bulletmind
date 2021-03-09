/*
 * Copyright (c) 2021 Paul Hindt <phindt@gmail.com>
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

#if defined(BM_WINDOWS)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#endif

#include "toml_config.h"

#include <errno.h>

bool read_toml_config(const char *path, toml_table_t **toml)
{
	FILE *fp = NULL;
	toml_table_t *conf = NULL;
	char err_buf[256];

	fp = fopen(path, "r");
	if (fp == NULL) {
		printf("Error opening TOML file %s: %d\n", path, errno);
		return false;
	}

	conf = toml_parse_file(fp, err_buf, sizeof(err_buf));

	fclose(fp);

	if (!conf) {
		printf("Error parsing TOML file %s: %s\n", path, err_buf);
		return false;
	}

	*toml = conf;

	return true;
}

// toml does allocation inside toml_raw_to_X functions (i.e. toml_rtos)
bool read_table_string(toml_table_t *table, const char *key, char **val)
{
	if (table != NULL) {
		const char *raw_value = toml_raw_in(table, key);
		char *tmp = NULL;

		if (raw_value != NULL)
			toml_rtos(raw_value, &tmp);

		*val = realloc(*val, (sizeof(char) * strlen(tmp)) + 1);
		if (tmp != NULL)
			memcpy(*val, tmp, sizeof(char) * strlen(tmp) + 1);

		free(tmp);

		return true;
	}

	return false;
}

bool read_table_int32(toml_table_t *table, const char *key, i32 *val)
{
	if (table != NULL) {
		const char *raw_value = toml_raw_in(table, key);
		i64 tmp = 0LL;
		if (raw_value != NULL) {
			toml_rtoi(raw_value, &tmp);
			*val = (i32)tmp;
		}
		return true;
	}

	return false;
}

bool read_table_f64(toml_table_t *table, const char *key, f64 *val)
{
	if (table != NULL) {
		const char *raw_value = toml_raw_in(table, key);
		f64 tmp = 0.0;
		if (raw_value != NULL) {
			toml_rtod(raw_value, &tmp);
			*val = tmp;
		}
		return true;
	}

	return false;
}

#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif
