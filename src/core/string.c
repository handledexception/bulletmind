#include "core/binary.h"
#include "core/string.h"
#include "core/memory.h"

#include <string.h>

#define CSTR_EMPTY ""
#define WSTR_EMPTY L""

struct string* str_new(size_t size)
{
	struct string* s = MEM_ALLOC(sizeof(*s));
	str_init(s);
	if (size > 0)
		str_reserve(s, size);
	return s;
}

void str_free(struct string* s)
{
	if (s != NULL) {
		BM_MEM_FREE(s);
		s = NULL;
	}
}

void str_init(string_t* s)
{
	s->capacity = 0;
	s->size = 0;
	s->data = NULL;
	s->refs = NULL;
	memset(s->text, 0, SSO_SIZE);
}

void str_reserve(string_t* s, size_t capacity)
{
	if (s != NULL) {
		if (capacity <= s->capacity)
			return;
		capacity = bit_round(capacity + 1) - 1;
		if (s->capacity < SSO_SIZE) {
			char tmp[SSO_SIZE];
			memcpy(tmp, s->text, SSO_SIZE);
			s->capacity = capacity + 1;
			s->data = MEM_ALLOC(s->capacity);
			memcpy(s->data, tmp, SSO_SIZE);
		} else {
			s->capacity = capacity + 1;
			s->data = mem_realloc(s->data, s->capacity);
		}
	}
}

void str_resize(string_t* s, size_t new_size)
{
	if (new_size == s->capacity)
		return;
	else if (new_size < s->capacity) {

	} else {

	}
}

void str_assign(string_t* s, const char* text)
{
	size_t len = strlen(text);
	str_reserve(s, len);
	if (len < SSO_SIZE) {
		memcpy(s->text, text, len);
		s->capacity = SSO_SIZE - 1;
	} else {
		memcpy(s->data, text, len);
		s->capacity = len;
	}
}

void str_copy(string_t* dst, string_t* src)
{
	str_reserve(dst, src->capacity);
	if (dst->capacity < SSO_SIZE)
		memcpy(dst->text, src->text, src->capacity);
	else
		memcpy(dst->data, src->data, src->capacity);
}

bool cstr_contains(const char* str, const char* substring)
{
	if (str == substring)
		return true;
	if (!str || !substring)
		return false;
	size_t sub_len = strlen(substring);
	if (strlen(str) < sub_len)
		return false;
	for (size_t i = 0; i < strlen(str); i++) {
		size_t matches = 0;
		for (size_t j = 0; j < sub_len; j++) {
			if (str[i + matches] != substring[j])
				break;
			else
				matches++;
			if (matches == sub_len)
				return true;
		}
	}
	return false;
}

char* cstr_upper_no_copy(char* str, size_t len)
{
	if (!str)
		return str;
	if (!*str)
		return str;
	size_t slen = len ? len : strlen(str);
	for (size_t i = 0; i < slen; i++) {
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] -= 0x20;
	}
	return str;
}

char* cstr_lower_no_copy(char* str, size_t len)
{
	if (!str)
		return str;
	if (!*str)
		return str;
	char* tmp = str;
	size_t slen = len ? len : strlen(tmp);
	for (size_t i = 0; i < slen; i++) {
		if (tmp[i] >= 'A' && tmp[i] <= 'Z')
			tmp[i] += 0x20;
	}
	return tmp;
}

int cstr_first_index_of(const char* str, size_t len, const char c)
{
	if (!str)
		return -1;
	if (!*str)
		return -1;
	size_t slen = len ? len : strlen(str);
	for (size_t i = 0; i < slen; i++) {
		if (str[i] == c)
			return (int)i;
	}
	return -1;
}

int cstr_last_index_of(const char* str, size_t len, const char c)
{
	if (!str)
		return -1;
	if (!*str)
		return -1;
	size_t slen = len ? len : strlen(str);
	for (size_t i = slen; i >= 0; i--) {
		if (str[i] == c)
			return (int)i;
	}
	return -1;
}

int astrcmp_n(const char* str1, const char* str2, size_t n)
{
	if (!n)
		return 0;
	if (!str1)
		str1 = CSTR_EMPTY;
	if (!str2)
		str2 = CSTR_EMPTY;

	do {
		char ch1 = *str1;
		char ch2 = *str2;
		if (ch1 < ch2)
			return -1;
		else if (ch1 > ch2)
			return 1;
	} while (*str1++ && *str2++ && --n);

	return 0;
}

int wstrcmp_n(const wchar_t* str1, const wchar_t* str2, size_t n)
{
	if (!n)
		return 0;
	if (!str1)
		str1 = WSTR_EMPTY;
	if (!str2)
		str2 = WSTR_EMPTY;
	do {
		wchar_t ch1 = *str1;
		wchar_t ch2 = *str2;
		if (ch1 < ch2)
			return -1;
		else if (ch1 > ch2)
			return 1;
	} while (*str1++ && *str2++ && --n);

	return 0;
}
