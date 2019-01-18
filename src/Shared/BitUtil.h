#ifndef BIT_UTIL_H_
#define BIT_UTIL_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool is_open;
    bool has_ended;
    char* source;
    uint32_t cur_byte;
    uint32_t byte_size;
    uint8_t cur_bit;
    char bit_buffer;
} BIT_READ_STREAM;

typedef struct {
    bool is_open;
    FILE* dest;
    uint8_t cur_bit;
    uint32_t wrote_bytes;
    char bit_buffer;
} BIT_WRITE_STREAM;

BIT_READ_STREAM* open_bit_read_stream(char* source_buffer, uint32_t source_size);
int read_bit(BIT_READ_STREAM* stream, bool* bit_buffer);
int skip_bit(BIT_READ_STREAM* stream);
void close_bit_read_stream(BIT_READ_STREAM* stream);

BIT_WRITE_STREAM* open_bit_write_stream(FILE* dest_file);
int write_bit(BIT_WRITE_STREAM* stream, bool bit);
int flush_bit_write_stream(BIT_WRITE_STREAM* stream, uint8_t* offset_buffer);
void close_bit_write_stream(BIT_WRITE_STREAM* stream);

#endif
