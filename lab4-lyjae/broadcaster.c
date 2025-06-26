#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#define PTS_PATH "/dev/pts"
#define MAX_PATH_LEN 256

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"<message>\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char formatted_msg[1024];
    snprintf(formatted_msg, sizeof(formatted_msg), "[Broadcast] %s\n", argv[1]);

    DIR *dir = opendir(PTS_PATH);
    if (!dir) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        int is_numeric = 1;
        for (int i = 0; entry->d_name[i] != '\0'; i++) {
            if (!isdigit((unsigned char)entry->d_name[i])) {
                is_numeric = 0;
                break;
            }
        }
        if (!is_numeric)
            continue;

        char pts_device[MAX_PATH_LEN];
        snprintf(pts_device, sizeof(pts_device), "%s/%s", PTS_PATH, entry->d_name);

        int fd = open(pts_device, O_WRONLY);
        if (fd < 0)
            continue;

        write(fd, formatted_msg, strlen(formatted_msg));
        close(fd);
    }

    closedir(dir);
    return 0;
}

