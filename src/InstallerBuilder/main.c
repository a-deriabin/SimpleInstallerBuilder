#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../Shared/StringUtil.h"
#include "../Shared/FileExtracter.h"
#include "../Shared/FileToExeWriter.h"
#include "../Shared/ArrayList.h"

#define DEBUG false

static void print_extract_error(FILE_EXTRACT_RESULT result) {
    switch (result) {
        case EXTRACT_OPEN_SOURCE_ERROR:
            printf("Failed to open source file.\n");
            break;
        case EXTRACT_OPEN_DEST_ERROR:
            printf("Failed to open destination file.\n");
            break;
        case EXTRACT_READ_ERROR:
            printf("Failed to read from source file.\n");
            break;
        case EXTRACT_WRITE_ERROR:
            printf("Failed to write to destination file.\n");
            break;
        case EXTRACT_DECOMPRESS_ERROR:
            printf("Failed to decompress extracted data.\n");
            break;
        case EXTRACT_SUCCESS:
            printf("Everything's ok.\n");
            break;
    }
}

static void print_append_error(FILE_APPEND_RESULT result) {
    switch (result) {
        case FILE_APPEND_SUCCESS:
            printf("Finished successfully.\n");
            break;
        case FILE_APPEND_OPEN_DEST_ERROR:
            printf("Failed to open destination file.\n");
            break;
        case FILE_APPEND_OPEN_SOURCE_ERROR:
            printf("Failed to open one of the source files.\n");
            break;
        case FILE_APPEND_WRITE_ERROR:
            printf("Failed to write to a destination file.\n");
            break;
        case FILE_APPEND_READ_ERROR:
            printf("Failed to read from a source file.\n");
            break;
        case FILE_APPEND_COMPRESS_ERROR:
            printf("Failed to compress a file.\n");
            break;
    }
}

static void parse_args(int argc, char** argv,
         char** list_file_name, bool* include_uninstaller) {

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            *list_file_name = argv[i + 1];
            i += 1;
            continue;
        }
        
        if (strcmp(argv[i], "-u") == 0) {
            if (*argv[i+1] == 'Y' || *argv[i+1] == 'y')
                *include_uninstaller = true;
            else if (*argv[i+1] == 'N' || *argv[i+1] == 'n')
                *include_uninstaller = false;
            else
                printf("Unknown option %s. Expected Y or N.\n", argv[i+1]);
            i += 1;
            continue;
        }
        
        printf("Unknown argument: %s\n", argv[i]);
    }
}

/*
    InstallerBuilder entry point.
    Input: list of files and configuration.
    Output: single installer file (.exe).

    Command-line arguments:
        -l <path_to_file>  --  specify path to files list file [optional]
        -u [Y/N]        --  include uninstaller? [optional, yes by default]

    List of files should be a text file, where each line
    represents a single file or directory.
    If it is a file, it will be copied to the root of destination directory.
    If it is a directory, the files will preserve their relative path.
    E.g. we write: '/mydir' to install_files.txt.
    mydir is a directory with subdirectory 'test' and file 'a.txt'.
    Resulting installer output will be: USER_SPECIFIED_DIR/test/a.txt
*/
int main(int argc, char** argv)
{
    // Get settings
    char* list_file_name = "install_files.txt";
    bool include_uninstaller = true;
    parse_args(argc, argv, &list_file_name, &include_uninstaller);

    // Extract files of current file
    char* cur_file = argv[0];
    FILE_EXTRACT_RESULT extract_result = extract_files(cur_file, ".");
    if (extract_result != EXTRACT_SUCCESS) {
        printf("Failed to extract files: ");
        print_extract_error(extract_result);
        return 0;
    }
    printf("Extracted files successfully.\n");

    // Get list of files to be packed
    FILE *list_file = fopen(list_file_name, "r");
    if (list_file == NULL) {
        printf("Failed to open file: %s", list_file_name);
        return 0;
    }

    array_list* paths_list = list_new(sizeof(char**));

    // Read file paths line by line (one line is one path)
    char* line_buffer = (char*) malloc(sizeof(char) * 255);
    while (fgets(line_buffer, 255, list_file) != NULL) {
        char* trimmed_line = string_trim(line_buffer);
        if (string_is_empty(trimmed_line))
            continue;
        list_add(paths_list, &trimmed_line);

        #if DEBUG
        printf("DBG: got path: %s\n", trimmed_line);
        #endif
    }

    // Add uninstaller to list if needed
    if (include_uninstaller) {
        char* uninstaller_str = string_copy("Uninstall.exe");
        list_add(paths_list, &uninstaller_str);
    }

    fclose(list_file);

    // Convert list to array
    size_t path_count = (size_t) paths_list->size;
    char** paths_arr = (char**) malloc(sizeof(char*) * path_count);

    #if DEBUG
    printf("DBG: Path count: %u\n", path_count);
    #endif

    for (size_t i = 0; i < path_count; i++) {
        char* file = *((char**)list_get(paths_list, i));
        char* fcopy = string_copy(file);
        paths_arr[i] = fcopy;

        #if DEBUG
        printf("DBG: file %u: ", i);
        printf("%s\n", fcopy);
        #endif
    }

    // Append each file from the list into installer executable
    FILE_APPEND_RESULT append_result = write_files("Installer.exe", paths_arr, path_count);
    if (append_result != FILE_APPEND_SUCCESS) {
        printf("Installer generation error: ");
        print_append_error(append_result);
    }

    // Remove unneeded files and finish
    remove("Uninstall.exe");
    if (append_result != FILE_APPEND_SUCCESS)
        remove("Installer.exe");
    else
        printf("Finished successfully.\n");

    return 0;
}
