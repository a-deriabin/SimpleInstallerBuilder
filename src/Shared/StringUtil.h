#ifndef STRING_UTIL_H_
#define STRING_UTIL_H_

#include <stdbool.h>

char* string_create_empty();
char* string_copy(const char* str_to_copy);
char* string_concat(const char* str_a, const char* str_b);
bool string_is_empty(const char* str);
char* string_trim(const char* str);
char last_char(const char* str);

#endif