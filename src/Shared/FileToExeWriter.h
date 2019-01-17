#ifndef FILE_TO_EXE_WRITER_H_
#define FILE_TO_EXE_WRITER_H_

typedef enum { 
    FILE_APPEND_SUCCESS,
    FILE_APPEND_OPEN_DEST_ERROR,
    FILE_APPEND_OPEN_SOURCE_ERROR,
    FILE_APPEND_WRITE_ERROR,
    FILE_APPEND_READ_ERROR
} FILE_APPEND_RESULT;

extern FILE_APPEND_RESULT write_files(const char* const dest_file,
                char** files_to_write,
                const size_t write_file_count);

#endif