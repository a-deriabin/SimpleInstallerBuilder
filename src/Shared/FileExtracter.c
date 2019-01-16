#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <direct.h>
#include <libgen.h>

#include "FileExtracter.h"
#include "FileUtil.h"
#include "StringUtil.h"


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
    fseek(from, -sizeof(char) * name_length, SEEK_CUR);

    printf("DBG: dest name str: %s\n", name_str);
    printf("DBG: name length: %u\n", name_length);

    // Read file size
    uint32_t file_size;
    fseek(from, -sizeof(uint32_t), SEEK_CUR);
    read_result = fread(&file_size, sizeof(uint32_t), 1, from);
    fseek(from, -sizeof(uint32_t), SEEK_CUR);

    // Read the whole file into buffer
    char* buffer = (char*) malloc(file_size);
    fseek(from, -sizeof(char) * file_size, SEEK_CUR);
    read_result = fread(buffer, sizeof(char) * file_size, 1, from);
    fseek(from, -sizeof(char) * file_size, SEEK_CUR);

    // Make full file path
    char* path_str = path_combine(dest_dir, name_str);
    printf("DBG: output file is: %s\n", path_str);

    // Ensure directory exists
    char* path_str_copy = string_copy(path_str);
    char* path_dir = dirname(path_str_copy);
    mkdir(path_dir);
    printf("Created dest dir: %s\n", path_dir);

    // Write to a new file
    FILE* out_file = fopen(path_str, "wb");
    if (out_file == NULL)
        return EXTRACT_OPEN_DEST_ERROR;

    int write_result = fwrite(buffer, sizeof(char) * file_size, 1, out_file);
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

    // Get file size
    //long file_size = ftell(in_file);

    // Get number of files stored in specified file
    uint32_t file_count = 0;
    fseek(in_file, -sizeof(uint32_t), SEEK_CUR);
    int read_result = fread(&file_count, sizeof(uint32_t), 1, in_file);
    if (read_result < 1) {
        fclose(in_file);
        return EXTRACT_READ_ERROR;
    }
    fseek(in_file, -sizeof(uint32_t), SEEK_CUR);

    printf("DBG: file count: %u\n", file_count);


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
