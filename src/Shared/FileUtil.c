#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "FileUtil.h"
#include "ArrayList.h"
#include "StringUtil.h"

#define DEBUG true

int is_directory(const char* path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}

/*
    Returns a list of char* strings.
    Each string represents a path to a file or a directory
    within specified directory.
    Returns NULL in if failed to open directory.
*/
array_list* directory_entries(const char* dir_path) {
    DIR *dir;
    struct dirent *ent;
    array_list* list = list_new(sizeof(char**));

    if ((dir = opendir(dir_path)) == NULL)
        return NULL;

    while ((ent = readdir(dir)) != NULL) {
        char* dir_name = string_copy(ent->d_name);
        list_add(list, &dir_name);
    }

    closedir(dir);
    return list;
}

char* path_combine(const char* dir, const char* file) {
    char last_dir_char = last_char(dir);
    //printf("DBG: last_dir_char: %c\n", last_dir_char);
    //printf("DBG: of path; %s\n", dir);
    
    if (last_dir_char == '\0' || last_dir_char == '/' || last_dir_char == '\\') {
        return string_concat(dir, file);
    }
    else
        return string_concat(string_concat(dir, "\\"), file);
}

bool delete_recursively(const char* path) {
    printf("DBG: deleting path %s\n", path);

    if (is_directory(path)) {
        bool any_fails = false;

        array_list* entry_list = directory_entries(path);
        for (uint32_t i = 0; i < entry_list->size; i++) {
            char* entry_path = *((char**)list_get(entry_list, i));
            if (!strcmp(entry_path, ".") || !strcmp(entry_path, ".."))
                continue;

            printf("DBG: found file %s\n", entry_path);

            char* full_path = path_combine(path, entry_path);

            #if DEBUG
            printf("DBG: recursively delete directory %s\n", full_path);
            #endif
            if (!delete_recursively(full_path)) {
                any_fails = true;
                printf("Failed to delete %s\n", full_path);
            }
        }
        if (any_fails) {
            printf("DBG: Failed to clean directory. Skipping its deletion.\n");
            return 0;
        }
        else if (rmdir(path)) {
            printf("DBG: failed to delete directory %s\n", path);
            return 0;
        }
        #if DEBUG
        printf("DBG: delete directory %s\n", path);
        #endif
        return 1;
    }
    else {
        #if DEBUG
        printf("DBG: delete file %s\n", path);
        #endif
        return !remove(path);
    }
}
