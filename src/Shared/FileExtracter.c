#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <direct.h>
#include <libgen.h>

#include "FileExtracter.h"
#include "FileUtil.h"
#include "StringUtil.h"
#include "BitUtil.h"
#include "HuffmanCompression.h"

#define DEBUG false

static FILE_EXTRACT_RESULT extract_next(FILE* from, const char* dest_dir) {
    // Read name length
    uint16_t name_length;
    fseek(from, -sizeof(uint16_t), SEEK_CUR);
    int read_result = fread(&name_length, sizeof(uint16_t), 1, from);
    if (read_result < 1)
        return EXTRACT_READ_ERROR;
    fseek(from, -sizeof(uint16_t), SEEK_CUR);

    // Read name string
    char* name_str = (char*) malloc(sizeof(char) * name_length);
    fseek(from, -sizeof(char) * name_length, SEEK_CUR);
    read_result = fread(name_str, sizeof(char) * name_length, 1, from);
    if (read_result < 1)
        return EXTRACT_READ_ERROR;
    fseek(from, -sizeof(char) * name_length, SEEK_CUR);

    #if DEBUG
    printf("DBG: dest name str: %s\n", name_str);
    printf("DBG: name length: %u\n", name_length);
    #endif

    // Read compression marker
    char is_compressed = 0;
    fseek(from, -sizeof(char), SEEK_CUR);
    read_result = fread(&is_compressed, sizeof(char), 1, from);
    if (read_result < 1)
        return EXTRACT_READ_ERROR;
    fseek(from, -sizeof(char), SEEK_CUR);

    #if DEBUG
    printf("DBG: is compressed: %u\n", is_compressed);
    #endif

    // These should be filled during file extraction
    char* buffer = NULL;
    size_t file_size = 0;

    if (is_compressed) {
        #if DEBUG
        printf("DBG: decompression started\n");
        #endif

        // Read occurrence array
        huff_tree_node** occur_array = read_occurrence_array(from);
        if (occur_array == NULL)
            return EXTRACT_DECOMPRESS_ERROR;

        #if DEBUG
        printf("DBG: read occurrence array\n");
        #endif

        // Read uncompressed file size
        fseek(from, -sizeof(size_t), SEEK_CUR);
        read_result = fread(&file_size, sizeof(size_t), 1, from);
        if (read_result < 1)
            return EXTRACT_READ_ERROR;
        fseek(from, -sizeof(size_t), SEEK_CUR);

        #if DEBUG
        printf("DBG: done read file size: %u bytes\n", file_size);
        #endif

        // Read byte count
        uint32_t byte_count = 0;
        fseek(from, -sizeof(uint32_t), SEEK_CUR);
        read_result = fread(&byte_count, sizeof(uint32_t), 1, from);
        if (read_result < 1)
            return EXTRACT_READ_ERROR;
        fseek(from, -sizeof(uint32_t), SEEK_CUR);

        // Read bit offset
        uint8_t bit_offset = 0;
        fseek(from, -sizeof(uint8_t), SEEK_CUR);
        read_result = fread(&bit_offset, sizeof(uint8_t), 1, from);
        if (read_result < 1)
            return EXTRACT_READ_ERROR;
        fseek(from, -sizeof(uint8_t), SEEK_CUR);

        #if DEBUG
        printf("DBG: read %u offset value.\n", bit_offset);
        #endif

        // Read compressed data
        char* compressed_buffer = (char*)malloc(byte_count);
        if (compressed_buffer == NULL)
            return EXTRACT_DECOMPRESS_ERROR;
        fseek(from, -sizeof(char) * byte_count, SEEK_CUR);
        read_result = fread(compressed_buffer, sizeof(char) * byte_count, 1, from);
        if (read_result < 1)
            return EXTRACT_READ_ERROR;
        fseek(from, -sizeof(char) * byte_count, SEEK_CUR);

        #if DEBUG
        printf("DBG: read %u compressed bytes.\n", byte_count);
        #endif

        // Build code tree
        huff_tree_node* tree_root = build_tree(occur_array);
        if (tree_root == NULL)
            return EXTRACT_DECOMPRESS_ERROR;

        #if DEBUG
        printf("DBG: done build code tree\n");
        #endif

        // Open bit stream
        BIT_READ_STREAM* r_stream = open_bit_read_stream(
                compressed_buffer, byte_count, bit_offset);
        if (r_stream == NULL)
            return EXTRACT_DECOMPRESS_ERROR;

        #if DEBUG
        printf("DBG: done open read stream\n");
        #endif

        // Decompress
        buffer = read_and_decompress(file_size, tree_root, r_stream);
        if (buffer == NULL)
            return EXTRACT_DECOMPRESS_ERROR;

        #if DEBUG
        printf("DBG: done read_and_decompress\n");
        #endif

        // Close bit stream
        close_bit_read_stream(r_stream);
    }
    else {
        // Read file size
        fseek(from, -sizeof(size_t), SEEK_CUR);
        read_result = fread(&file_size, sizeof(size_t), 1, from);
        if (read_result < 1)
            return EXTRACT_READ_ERROR;
        fseek(from, -sizeof(size_t), SEEK_CUR);

        #if DEBUG
        printf("DBG: read file size: %u\n", file_size);
        #endif

        // Read the whole file into buffer
        buffer = (char*) malloc(file_size);
        fseek(from, -sizeof(char) * file_size, SEEK_CUR);
        read_result = fread(buffer, sizeof(char) * file_size, 1, from);
        if (read_result < 1)
            return EXTRACT_READ_ERROR;
        fseek(from, -sizeof(char) * file_size, SEEK_CUR);
    }

    // Make full file path
    char* path_str = path_combine(dest_dir, name_str);
    #if DEBUG
    printf("DBG: output file is: %s\n", path_str);
    #endif

    // Ensure directory exists
    char* path_str_copy = string_copy(path_str);
    char* path_dir = dirname(path_str_copy);
    mkdir(path_dir);
    #if DEBUG
    printf("Created dest dir: %s\n", path_dir);
    #endif

    // Free memory
    free(name_str);
    free(path_str_copy);

    #if DEBUG
    printf("DBG: freed some memory.\n");
    #endif

    // Write to a new file
    FILE* out_file = fopen(path_str, "wb");
    if (out_file == NULL)
        return EXTRACT_OPEN_DEST_ERROR;

    int write_result = fwrite(buffer, sizeof(char) * file_size, 1, out_file);
    free(path_str);
    free(buffer);

    if (write_result < 1) {
        fclose(out_file);
        return EXTRACT_WRITE_ERROR;
    }

    fclose(out_file);
    return EXTRACT_SUCCESS;
}

FILE_EXTRACT_RESULT extract_files(const char* from_file, const char* dest_dir) {
    FILE* in_file = fopen(from_file, "rb");
    if (in_file == NULL)
        return EXTRACT_OPEN_SOURCE_ERROR;

    // Move to the end
    fseek(in_file, 0, SEEK_END);

    // Get number of files stored in specified file
    uint32_t file_count = 0;
    fseek(in_file, -sizeof(uint32_t), SEEK_CUR);
    int read_result = fread(&file_count, sizeof(uint32_t), 1, in_file);
    if (read_result < 1) {
        fclose(in_file);
        return EXTRACT_READ_ERROR;
    }
    fseek(in_file, -sizeof(uint32_t), SEEK_CUR);

    #if DEBUG
    printf("DBG: file count: %u\n", file_count);
    #endif


    // Extract files
    for (uint32_t i = 0; i < file_count; i++) {
        FILE_EXTRACT_RESULT result = extract_next(in_file, dest_dir);
        if (result != EXTRACT_SUCCESS) {
            fclose(in_file);
            return result;
        }
    }

    fclose(in_file);
    return EXTRACT_SUCCESS;
}
