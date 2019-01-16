#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

#include "StringUtil.h"

char* string_create_empty() {
    char* str = (char*) malloc(sizeof(char));
    str[0] = '\0';
    return str;
}

/*char* string_copy(const char* str_to_copy) {
    if (str_to_copy == NULL)
        return string_create_empty();

    size_t length = strlen(str_to_copy) + 1;
    printf("DBG_STRUTIL: got length: %u\n", length);
    printf("str: %s\n", str_to_copy);
    size_t to_alloc = sizeof(char) * length;
    printf("trying to allocate %u bytes\n", to_alloc);
    char* new_str = (char*) malloc(to_alloc);
    if (new_str == NULL) {
        printf("failed to malloc\n");
        return string_create_empty();
    }
    printf("DBG_STRUTIL: malloc successful\n");
    strcpy(new_str, str_to_copy);
    printf("DBG_STRUTIL: strcpy successful\n");
    return new_str;
}*/

char* string_copy(const char* str_to_copy) {
    size_t length = strlen(str_to_copy) + 1;
    char* new_str = (char*) malloc(sizeof(char) * length);
    strcpy(new_str, str_to_copy);
    return new_str;
}

char* string_concat(const char* str_a, const char* str_b) {
    if (str_a == NULL && str_b == NULL)
        return string_create_empty();
    if (str_a == NULL)
        return string_copy(str_b);
    if (str_b == NULL)
        return string_copy(str_a);

    size_t length_a = strlen(str_a);
    size_t length_b = strlen(str_b);
    size_t length_res = length_a + length_b;

    char* new_str = (char*) malloc(sizeof(char) * length_res);
    strcpy(new_str, str_a);
    strcat(new_str, str_b);
    return new_str;
}

bool string_is_empty(const char* str) {
    size_t length = strlen(str);
    return length == 0;
}

char* string_trim(const char* str) {
    size_t length = strlen(str);
    size_t from = 0;
    size_t until = length - 1;

    for (int i = 0; i < length; i++) {
        if (isspace(str[i]))
            continue;
        from = i;
        break;
    }

    for (int i = length - 1; i >= from; i--) {
        if (isspace(str[i]))
            continue;
        until = i;
        break;
    }

    size_t new_length = until - from + 1;
    char* new_str = (char*) malloc(sizeof(char) * (new_length + 1));
    memcpy(new_str, str + from, sizeof(char) * new_length);
    new_str[new_length] = '\0';
    return new_str;
}

char last_char(const char* str) {
    if (str == NULL)
        return '\0';
    size_t length = strlen(str);
    if (length == 0)
        return '\0';
    return str[length - 1];
}