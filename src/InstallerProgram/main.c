#include <stdio.h>
#include <stdlib.h>
#include "../Shared/FileExtracter.h"


static void print_result_msg(FILE_EXTRACT_RESULT result) {
    switch (result) {
        case EXTRACT_SUCCESS:
            printf("Installed successfully.\n");
            break;

        case EXTRACT_OPEN_SOURCE_ERROR:
            printf("Error: failed to open source file.\n");
            break;

        case EXTRACT_OPEN_DEST_ERROR:
            printf("Error: failed to open destination file.\n");
            break;

        case EXTRACT_READ_ERROR:
            printf("Error: failed to read from file.\n");
            break;

        case EXTRACT_WRITE_ERROR:
            printf("Error: failed to write to file.\n");
            break;
    }
}

/*
    Installer entry point.
    Opens GUI form, which handles user input and calls installer core functions.
*/
int main(int argc, char** argv)
{
    //TODO: GUI form

    printf("Enter destination directory path: ");
    char* dest_path = (char*)malloc(255);
    if (!scanf("%s", dest_path)) {
        printf("Failed to get directory path.\n");
        return 0;
    }

    char* cur_file = argv[0];
    FILE_EXTRACT_RESULT result = extract_files(cur_file, dest_path);
    print_result_msg(result);
    return 0;
}