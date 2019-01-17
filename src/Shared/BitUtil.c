#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "BitUtil.h"

BIT_READ_STREAM* open_bit_read_stream(char* source_buffer, uint32_t source_size) {
    if (source_buffer == NULL)
        return NULL;
    if (source_size < 1)
        return NULL;

    BIT_READ_STREAM* stream = (BIT_READ_STREAM*) malloc(sizeof(BIT_READ_STREAM));
    stream->is_open = true;
    stream->has_ended = false;
    stream->source = source_buffer;
    stream->cur_byte = 0;
    stream->byte_size = source_size;
    stream->cur_bit = 0;
    stream->bit_buffer = source_buffer[0];
    return stream;
}

int read_bit(BIT_READ_STREAM* stream, bool* bit_buffer) {
    if (stream == NULL)
        return -1;
    if (!stream->is_open)
        return -2;
    if (bit_buffer == NULL)
        return -3;
    if (stream->has_ended)
        return -4;
    if (stream->source == NULL)
        return -5;

    const char BIT_MASK = 1;

    *bit_buffer = stream->bit_buffer & BIT_MASK;
    stream->bit_buffer >>= 1;
    stream->cur_bit += 1;
    if (stream->cur_bit == 8) {
        if (stream->cur_byte == stream->byte_size) {
            stream->has_ended = true;
        }
        else {
            stream->cur_byte += 1;
            stream->bit_buffer = stream->source[stream->cur_byte];
        }
    }

    return 0;
}

void close_bit_read_stream(BIT_READ_STREAM* stream) {
    if (stream == NULL)
        return;
    stream->is_open = false;
    free(stream);
}



BIT_WRITE_STREAM* open_bit_write_stream(FILE* dest_file) {
    if (dest_file == NULL)
        return NULL;

    BIT_WRITE_STREAM* stream = (BIT_WRITE_STREAM*) malloc(sizeof(BIT_WRITE_STREAM));
    stream->is_open = true;
    stream->dest = dest_file;
    stream->cur_bit = 0;
    stream->bit_buffer = 0;

    return stream;
}

int write_bit(BIT_WRITE_STREAM* stream, bool bit) {
    if (stream == NULL)
        return -1;
    if (!stream->is_open)
        return -2;
    if (stream->dest == NULL)
        return -3;

    const char BIT_MASK = -80; // 1000 0000

    stream->bit_buffer |= BIT_MASK & bit;

    if (stream->cur_bit < 7) {
        stream->cur_bit += 1;
        stream->bit_buffer >>= 1;
    }
    else {
        if (fwrite(&(stream->bit_buffer), sizeof(char), 1, stream->dest) < 1)
            return -4;
        stream->cur_bit = 0;
        stream->bit_buffer = 0;
    }

    return 0;
}

int flush_bit_write_stream(BIT_WRITE_STREAM* stream, uint8_t* offset_buffer) {
    uint8_t offset = 0;
    int write_result = 0;
    while (stream->cur_bit != 0) {
        write_result = write_bit(stream, 0);
        if (write_result != 0)
            return write_result;
        offset += 1;
    }

    *offset_buffer = offset;
    return 0;
}

void close_bit_write_stream(BIT_WRITE_STREAM* stream) {
    if (stream == NULL)
        return;
    if (!stream->is_open)
        return;
    
    stream->is_open = false;
    free(stream);
}
