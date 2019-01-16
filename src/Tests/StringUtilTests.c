#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Shared/StringUtil.h"

static int test_trim_1() {
    char* str = "hello";
    char* trimmed = string_trim(str);
    return strcmp(trimmed, "hello");
}

static int test_trim_2() {
    char* str = "  hello";
    char* trimmed = string_trim(str);
    return strcmp(trimmed, "hello");
}

static int test_trim_3() {
    char* str = "hello ";
    char* trimmed = string_trim(str);
    return strcmp(trimmed, "hello");
}

static int test_trim_4() {
    char* str = "hello\r\n";
    char* trimmed = string_trim(str);
    return strcmp(trimmed, "hello");
}

static int test_trim_5() {
    char* str = "\nhello\n";
    char* trimmed = string_trim(str);
    return strcmp(trimmed, "hello");
}

static int test_trim_6() {
    char* str = "hel lo";
    char* trimmed = string_trim(str);
    return strcmp(trimmed, "hel lo");
}

int main() {
    printf("TRIM tests:\n");
    printf("1. %d\n", test_trim_1() == 0);
    printf("2. %d\n", test_trim_2() == 0);
    printf("3. %d\n", test_trim_3() == 0);
    printf("4. %d\n", test_trim_4() == 0);
    printf("5. %d\n", test_trim_5() == 0);
    printf("6. %d\n", test_trim_6() == 0);

    return 0;
}