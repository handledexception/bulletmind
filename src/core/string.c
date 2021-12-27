#include "core/string.h"

#include <string.h>

static const char* astr_empty = "";
static const wchar_t* wstr_empty = L"";

bool str_contains(const char* s, const char* substring)
{
	if (s == substring)
		return true;
	if (!s || !substring)
		return false;
	size_t sub_len = strlen(substring);
	if (strlen(s) < sub_len)
		return false;
	for (size_t i = 0; i < strlen(s); i++) {
		size_t matches = 0;
		for (size_t j = 0; j < sub_len; j++) {
			if (s[i+matches] != substring[j])
				break;
			else
				matches++;
			if (matches == sub_len)
				return true;
		}
	}
	return false;
}

void str_upper_no_copy(char* s, size_t len)
{
	size_t slen = len ? len : strlen(s);
	for (size_t i = 0; i < slen; i++) {
		if (s[i] >= 'a' && s[i] <= 'z')
			s[i] -= 0x20;
	}
}

void str_lower_no_copy(char* s, size_t len)
{
	size_t slen = len ? len : strlen(s);
	for (size_t i = 0; i < slen; i++) {
		if (s[i] >= 'A' && s[i] <= 'Z')
			s[i] += 0x20;
	}
}

int str_first_index_of(const char* s, size_t len, const char c)
{
	size_t slen = len ? len : strlen(s);
	for (size_t i = 0; i < slen; i++) {
		if (s[i] == c)
			return (int)i;
	}
	return -1;
}

int str_last_index_of(const char* s, size_t len, const char c)
{
	size_t slen = len ? len : strlen(s);
	for (size_t i = slen; i >= 0; i--) {
		if (s[i] == c)
			return (int)i;
	}
	return -1;
}

int astrcmp_n(const char* str1, const char* str2, size_t n)
{
	if (!n)
		return 0;
	if (!str1)
		str1 = astr_empty;
	if (!str2)
		str2 = astr_empty;

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
		str1 = wstr_empty;
	if (!str2)
		str2 = wstr_empty;

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
