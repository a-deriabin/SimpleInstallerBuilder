#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../Shared/BitUtil.h"

static int read_test1() {
    char buffer[1];
    buffer[0] = -1;
    bool bit_buffer = 0;
    int read_result = 0;

    BIT_READ_STREAM* stream = open_bit_read_stream(buffer, 1, 0);
    for (int i = 0; i < 8; i++) {
        read_result = read_bit(stream, &bit_buffer);
        if (read_result != 0) {
            printf("Read bit error: %d\n", read_result);
            close_bit_read_stream(stream);
            return 0;
        }

        if (!bit_buffer) {
            printf("%d-th bit was 0 but expected 1.", i);
            close_bit_read_stream(stream);
            return 0;
        }
    }

    if (!stream->has_ended) {
        printf("Read all bits but stream isn't marked as ended.\n");
        return 0;
    }

    close_bit_read_stream(stream);
    return 1;
}

static int read_test2() {
    char buffer[3];
    buffer[0] = 0;
    buffer[1] = 0;
    buffer[2] = 0;
    bool bit_buffer = 0;
    int read_result = 0;

    BIT_READ_STREAM* stream = open_bit_read_stream(buffer, 3, 0);
    for (int i = 0; i < 24; i++) {
        read_result = read_bit(stream, &bit_buffer);
        if (read_result != 0) {
            printf("Read bit error: %d\n", read_result);
            close_bit_read_stream(stream);
            return 0;
        }

        if (bit_buffer) {
            printf("%d-ith bit was 1 but expected 0.");
            close_bit_read_stream(stream);
            return 0;
        }
    }

    if (!stream->has_ended) {
        printf("Read all bits but stream isn't marked as ended.\n");
        return 0;
    }

    close_bit_read_stream(stream);
    return 1;
}

static int read_test3() {
    char buffer[1];
    buffer[0] = 108; // 0110 1100
    bool bit_buffer = 0;
    int read_result = 0;
    bool expected[8] = { 0, 0, 1, 1, 0, 1, 1, 0 };

    BIT_READ_STREAM* stream = open_bit_read_stream(buffer, 1, 0);
    for (int i = 0; i < 8; i++) {
        read_result = read_bit(stream, &bit_buffer);
        if (read_result != 0) {
            printf("Read bit error: %d\n", read_result);
            close_bit_read_stream(stream);
            return 0;
        }

        if (bit_buffer != expected[i]) {
            printf("%d-th bit was ", i);
            printf("%d, but expected ", bit_buffer);
            printf("%d\n", expected[i]);
            close_bit_read_stream(stream);
            return 0;
        }
    }

    if (!stream->has_ended) {
        printf("Read all bits but stream isn't marked as ended.\n");
        return 0;
    }

    close_bit_read_stream(stream);
    return 1;
}

static int read_test4() {
    char buffer[1];
    buffer[0] = 108; // 0110 1100
    bool bit_buffer = 0;
    int read_result = 0;
    bool expected[8] = { 0, 0, 1, 1, 0, 1, 1, 0 };

    BIT_READ_STREAM* stream = open_bit_read_stream(buffer, 1, 3);
    for (int i = 0; i < 5; i++) {
        read_result = read_bit(stream, &bit_buffer);
        if (read_result != 0) {
            printf("Read bit error: %d\n", read_result);
            close_bit_read_stream(stream);
            return 0;
        }

        if (bit_buffer != expected[i]) {
            printf("%d-th bit was ", i);
            printf("%d, but expected ", bit_buffer);
            printf("%d\n", expected[i]);
            close_bit_read_stream(stream);
            return 0;
        }
    }

    if (!stream->has_ended) {
        printf("Read all bits but stream isn't marked as ended.\n");
        return 0;
    }

    close_bit_read_stream(stream);
    return 1;
}

static void test(int result, char* test_name) {
    if (result)
        printf("%s -- success\n", test_name);
    else
        printf("%s -- fail\n", test_name);
}

int main() {
    printf("BIT READ tests:\n");
    test(read_test1(), "read_test_1");
    test(read_test2(), "read_test_2");
    test(read_test3(), "read_test_3");
    test(read_test4(), "read_test_4");

    return 0;
}