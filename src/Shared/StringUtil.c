#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

#include "StringUtil.h"

char* string_copy(char* str_to_copy) {
    size_t length = strlen(str_to_copy);
    char* new_str = (char*) malloc(sizeof(char) * length);
    strcpy(new_str, str_to_copy);
    return new_str;
}

char* string_concat(char* str_a, char* str_b) {
    size_t length_a = strlen(str_a);
    size_t length_b = strlen(str_b);
    size_t length_res = length_a + length_b;

    char* new_str = (char*) malloc(sizeof(char) * length_res);
    strcpy(new_str, str_a);
    strcat(new_str, str_b);
    return new_str;
}

bool string_is_empty(char* str) {
    size_t length = strlen(str);
    return length == 0;
}

char* string_trim(char* str) {
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
