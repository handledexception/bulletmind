#include "core/binary.h"
#include "core/string.h"
#include "core/memory.h"

#include <string.h>

#define CSTR_EMPTY ""
#define WSTR_EMPTY L""

struct string* str_new(size_t size)
{
	struct string* s = BM_ALLOC(sizeof(*s));
	str_init(s);
	if (size > 0) {
		str_reserve(s, size);
	}
	return s;
}

void str_free(struct string* s)
{
	if (s != NULL) {
		str_clear(s);
		BM_FREE(s);
		s = NULL;
	}
}

void str_clear(string_t* s)
{
	if (s->capacity >= SSO_SIZE) {
		BM_FREE(s->data);
		s->data = NULL;
	} else {
		memset(s->data_sso, 0, SSO_SIZE);
	}
}

void str_init(string_t* s)
{
	s->data = NULL;
	s->capacity = SSO_SIZE - 1;
	s->size = 0;
}

void str_reserve(string_t* s, size_t capacity)
{
	if (s != NULL) {
		if (capacity <= s->capacity) {
			return;
		}
		capacity = bit_round(capacity + 1) - 1;
		if (s->capacity < SSO_SIZE) {
			char tmp[SSO_SIZE];
			strncpy(tmp, s->data_sso, SSO_SIZE);
			s->capacity = capacity + 1;
			s->data = BM_ALLOC(s->capacity);
			strncpy(s->data, tmp, SSO_SIZE);
		} else {
			s->capacity = capacity + 1;
			s->data = BM_REALLOC(s->data, s->capacity);
		}
	}
}

void str_resize(string_t* s, size_t new_size)
{
	if (new_size == s->capacity) {
		return;
	} else if (new_size < s->capacity) {
	} else {
	}
}

void str_copy(string_t* dst, const string_t* src)
{
	str_reserve(dst, src->capacity);
	if (dst->capacity < SSO_SIZE) {
		strncpy(dst->data_sso, src->data_sso, src->capacity);
	} else {
		strncpy(dst->data, src->data, src->capacity);
	}
}

void str_copy_cstr(string_t* dst, const char* src, size_t len)
{
	if (dst == NULL || src == NULL || *src == '\0') {
		return;
	}

	bool add_nul = false;
	if (src[len] != '\0') {
		len += 1;
		add_nul = true;
	}

	if (len >= SSO_SIZE) {
		dst->data = BM_REALLOC(dst->data, len);
		dst->capacity = len;
		dst->size = len;
		strncpy(dst->data, src, add_nul ? len-1 : len);
		if (add_nul) {
			dst->data[len-1] = '\0';
		}
	} else {
		strncpy(dst->data_sso, src, add_nul ? len-1 : len);
		if (add_nul) {
			dst->data_sso[len-1] = '\0';
		}
		dst->capacity = SSO_SIZE-1;
		dst->size = len;
	}
}

const char* str_get(string_t* s)
{
	if (s != NULL) {
		if (s->capacity >= SSO_SIZE) {
			return s->data;
		} else {
			return &s->data_sso[0];
		}
	} else {
		return NULL;
	}
}

u32 str_length(string_t* s)
{
	u32 sz = 0;
	if (s->capacity >= SSO_SIZE) {
		while (s->data != NULL && s->data[sz++] != '\0') {
			s->size = sz;
		}
	} else {
		return (u32)s->size;
	}
	return sz;
}

bool str_starts_with(const string_t* s, const char* prefix)
{
	if (s != NULL) {
		u32 len = str_length(s);
		u32 prefix_len = (u32)strlen(prefix);
		const char* data = str_get(s);
		if (prefix_len < len && prefix != NULL && data != NULL) {
			u32 found_count = 0;
			for (u32 i = 0; i < prefix_len; i++) {
				if (data[i] != prefix[i]) {
					break;
				} else {
					found_count++;
				}
			}
			return found_count == prefix_len;
		}
	}

	return false;
}

bool str_ends_with(const string_t* s, const char* suffix)
{
	if (s != NULL) {
		return true;
	}

	return false;
}

bool str_contains(const string_t* s, const char* substring)
{
	if (s != NULL) {
		return cstr_contains(str_get(s), substring);
	} else {
		return false;
	}
}

bool cstr_contains(const char* str, const char* substring)
{
	if (str == substring) {
		return true;
	}
	if (!str || !substring) {
		return false;
	}
	size_t sub_len = strlen(substring);
	if (strlen(str) < sub_len) {
		return false;
	}
	for (size_t i = 0; i < strlen(str); i++) {
		size_t matches = 0;
		for (size_t j = 0; j < sub_len; j++) {
			if (str[i + matches] != substring[j]) {
				break;
			} else {
				matches++;
			}
			if (matches == sub_len) {
				return true;
			}
		}
	}
	return false;
}

char* cstr_upper_no_copy(char* str, size_t len)
{
	if (!str) {
		return str;
	}
	if (!*str) {
		return str;
	}
	size_t slen = len ? len : strlen(str);
	for (size_t i = 0; i < slen; i++) {
		if (str[i] >= 'a' && str[i] <= 'z') {
			str[i] -= 0x20;
		}
	}
	return str;
}

char* cstr_lower_no_copy(char* str, size_t len)
{
	if (!str) {
		return str;
	}
	if (!*str) {
		return str;
	}
	char* tmp = str;
	size_t slen = len ? len : strlen(tmp);
	for (size_t i = 0; i < slen; i++) {
		if (tmp[i] >= 'A' && tmp[i] <= 'Z') {
			tmp[i] += 0x20;
		}
	}
	return tmp;
}

int cstr_first_index_of(const char* str, size_t len, const char c)
{
	if (!str) {
		return -1;
	}
	if (!*str) {
		return -1;
	}
	size_t slen = len ? len : strlen(str);
	for (size_t i = 0; i < slen; i++) {
		if (str[i] == c) {
			return (int)i;
		}
	}
	return -1;
}

int cstr_last_index_of(const char* str, size_t len, const char c)
{
	if (!str) {
		return -1;
	}
	if (!*str) {
		return -1;
	}
	size_t slen = len ? len : strlen(str);
	for (size_t i = slen; i >= 0; i--) {
		if (str[i] == c) {
			return (int)i;
		}
	}
	return -1;
}

int astrcmp_n(const char* str1, const char* str2, size_t n)
{
	if (!n) {
		return 0;
	}
	if (!str1) {
		str1 = CSTR_EMPTY;
	}
	if (!str2) {
		str2 = CSTR_EMPTY;
	}

	do {
		char ch1 = *str1;
		char ch2 = *str2;
		if (ch1 < ch2) {
			return -1;
		} else if (ch1 > ch2) {
			return 1;
		}
	} while (*str1++ && *str2++ && --n);

	return 0;
}

int wstrcmp_n(const wchar_t* str1, const wchar_t* str2, size_t n)
{
	if (!n) {
		return 0;
	}
	if (!str1) {
		str1 = WSTR_EMPTY;
	}
	if (!str2) {
		str2 = WSTR_EMPTY;
	}
	do {
		wchar_t ch1 = *str1;
		wchar_t ch2 = *str2;
		if (ch1 < ch2) {
			return -1;
		} else if (ch1 > ch2) {
			return 1;
		}
	} while (*str1++ && *str2++ && --n);

	return 0;
}