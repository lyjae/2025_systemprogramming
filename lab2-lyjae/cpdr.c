#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#define BUFSIZE 4096

void cp_file(const char *src, const char *dst);
void cp_dir(const char *src, const char *dst);

int main(int argc, char *argv[])
{
    struct stat st;
    if (argc != 3) {
        fprintf(stderr, "usage: %s source destination\n", argv[0]);
        exit(1);
    }
    if (stat(argv[1], &st) == -1) {
        perror(argv[1]);
        exit(1);
    }
    if (S_ISDIR(st.st_mode))
        cp_dir(argv[1], argv[2]);
    else if (S_ISREG(st.st_mode))
        cp_file(argv[1], argv[2]);
    else {
        fprintf(stderr, "Unsupported file type: %s\n", argv[1]);
        exit(1);
    }
    return 0;
}

void cp_file(const char *src, const char *dst)
{
    int in_fd, out_fd, n;
    char buf[BUFSIZE];
    struct stat st;

    if ((in_fd = open(src, O_RDONLY)) < 0) {
        perror(src);
        exit(1);
    }
    if (fstat(in_fd, &st) < 0) {
        perror(src);
        exit(1);
    }


    mode_t old_umask = umask(0);
    if ((out_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode)) < 0) {
        perror(dst);
        exit(1);
    }
    umask(old_umask);

    while ((n = read(in_fd, buf, BUFSIZE)) > 0)
        if (write(out_fd, buf, n) != n) {
            perror(dst);
            exit(1);
        }
    if (n < 0) {
        perror(src);
        exit(1);
    }
    close(in_fd);
    close(out_fd);

    chmod(dst, st.st_mode);
}

void cp_dir(const char *src, const char *dst)
{
    DIR *dp;
    struct dirent *entry;
    struct stat st, entry_st;
    char src_path[PATH_MAX], dst_path[PATH_MAX];

    if (stat(src, &st) == -1) {
        perror(src);
        exit(1);
    }
    if (mkdir(dst, st.st_mode) == -1) {
        if (errno != EEXIST) {
            perror(dst);
            exit(1);
        }
    }
    if ((dp = opendir(src)) == NULL) {
        perror(src);
        exit(1);
    }
    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;
        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dst_path, sizeof(dst_path), "%s/%s", dst, entry->d_name);
        if (stat(src_path, &entry_st) == -1) {
            perror(src_path);
            continue;
        }
        if (S_ISDIR(entry_st.st_mode))
            cp_dir(src_path, dst_path);
        else if (S_ISREG(entry_st.st_mode))
            cp_file(src_path, dst_path);
    }
    closedir(dp);

    chmod(dst, st.st_mode);
}
