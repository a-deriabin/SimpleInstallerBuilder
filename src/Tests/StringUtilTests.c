#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Shared/StringUtil.h"

static int test_trim_1() {
    char* str = "hello";
    char* trimmed = string_trim(str);
    return strcmp(trimmed, "hello") == 0;
}

static int test_trim_2() {
    char* str = "  hello";
    char* trimmed = string_trim(str);
    return strcmp(trimmed, "hello") == 0;
}

static int test_trim_3() {
    char* str = "hello ";
    char* trimmed = string_trim(str);
    return strcmp(trimmed, "hello") == 0;
}

static int test_trim_4() {
    char* str = "hello\r\n";
    char* trimmed = string_trim(str);
    return strcmp(trimmed, "hello") == 0;
}

static int test_trim_5() {
    char* str = "\nhello\n";
    char* trimmed = string_trim(str);
    return strcmp(trimmed, "hello") == 0;
}

static int test_trim_6() {
    char* str = "hel lo";
    char* trimmed = string_trim(str);
    return strcmp(trimmed, "hel lo") == 0;
}

static int test_last_char_1() {
    char* str = "abc";
    char c = last_char(str);
    return c == 'c';
}

static int test_last_char_2() {
    char* str = "";
    char c = last_char(str);
    return c == '\0';
}

static int test_last_char_3() {
    char c = last_char("abc");
    return c == 'c';
}

static int test_last_char_4() {
    char* str = "rtftftf";
    char c = last_char(str);
    return c == 'f';
}

static void test(int result, char* test_name) {
    if (result)
        printf("%s -- success\n", test_name);
    else
        printf("%s -- fail\n", test_name);
}

int main() {
    printf("TRIM tests:\n");
    test(test_trim_1(), "test_trim_1");
    test(test_trim_2(), "test_trim_2");
    test(test_trim_3(), "test_trim_3");
    test(test_trim_4(), "test_trim_4");
    test(test_trim_5(), "test_trim_5");
    test(test_trim_6(), "test_trim_6");

    printf("\nLAST_CHAR tests:\n");
    test(test_last_char_1(), "last_char_1");
    test(test_last_char_2(), "last_char_2");
    test(test_last_char_3(), "last_char_3");
    test(test_last_char_4(), "last_char_4");

    return 0;
}