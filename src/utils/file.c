/*
** file.c for Cherokee
**
** Made by BOURDALE Jules
** Login   <bourda_j@etna-alternance.net>
**
*/
#include <stdio.h>

#include "../log/log.h"
#include "file.h"

char *get_file_path(char *root_path, char *url) {
    char *file_path;

    file_path = malloc(strlen(root_path) + strlen(url) + 2);
    memset(file_path, '\0', strlen(root_path) + strlen(url) + 2);
    strcat(file_path, root_path);
    strcat(file_path, url);

    return file_path;
}

char *get_file_extension(const char *filename) {
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

void *read_file(const char *file_path, int *size) {
    char *buf;
    FILE *fd;

    log_debug("READING FILE : %s", file_path);
    fd = fopen(file_path,"rb");
    fseek(fd, 0, SEEK_END); //go to end
    *size = ftell(fd); //get position at end (length)
    fseek(fd, 0, SEEK_SET); //go to beg.
    buf = (char *)malloc(*size + 1); //malloc buffer
    memset(buf, '\0', *size + 1);
    fread(buf, *size, 1, fd); //read into buffer

    log_debug("BUF FILE : %s", buf);
    fclose(fd);

    return buf;
}
