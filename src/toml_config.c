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