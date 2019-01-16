#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "FileUtil.h"
#include "ArrayList.h"
#include "StringUtil.h"

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
