#if defined(BM_WINDOWS)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#endif

#include "toml_config.h"

#include <errno.h>

bool read_toml_config(const char* path, toml_table_t** toml) {
    FILE* fp = NULL;
    toml_table_t* conf = NULL;
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
bool read_table_string(toml_table_t* table, const char* key, char** string) {
    if (table != NULL) {
        const char* raw_value = toml_raw_in(table, key);
        char* tmp = NULL;

        if (raw_value != NULL)
            toml_rtos(raw_value, &tmp);

        *string = realloc(*string, (sizeof(char) * strlen(tmp))+1);
        if (tmp != NULL)
            memcpy(*string, tmp, sizeof(char) * strlen(tmp) + 1);

        free(tmp);

        return true;
    }

    return false;
}

bool read_table_int32(toml_table_t* table, const char* key, int32_t* i32) {
    if (table != NULL) {
        const char* raw_value = toml_raw_in(table, key);
        int64_t tmp = 0LL;
        if (raw_value != NULL) {
            toml_rtoi(raw_value, &tmp);
            *i32 = (int32_t)tmp;
        }
        return true;
    }

    return false;
}

bool read_table_double(toml_table_t* table, const char* key, double* dbl) {
    if (table != NULL) {
        const char* raw_value = toml_raw_in(table, key);
        double tmp = 0.0;
        if (raw_value != NULL) {
            toml_rtod(raw_value, &tmp);
            *dbl = tmp;
        }
        return true;
    }

    return false;
}

#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif
