#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define MAX_LINES 100
#define MAX_LINE_LEN 256
#define AUTOSAVE_FILE "autosave.txt"

char *lines[MAX_LINES];
int line_count = 0;

void save_lines() {
    FILE *fp = fopen(AUTOSAVE_FILE, "w");
    if (!fp) {
        perror("fopen");
        return;
    }
    for (int i = 0; i < line_count; i++) {
        fputs(lines[i], fp);
    }
    fclose(fp);
    printf("Autosaved %d lines to %s\n", line_count, AUTOSAVE_FILE);
    fflush(stdout);
}

void handle_autosave(int sig) {
    (void)sig;
    save_lines();
    alarm(5);
}

void handle_sigint(int sig) {
    (void)sig;
    printf("\nSIGINT received. Saving and exiting...\n");
    save_lines();
    exit(0);
}

int main() {
    signal(SIGALRM, handle_autosave);
    signal(SIGINT, handle_sigint);
    alarm(5);
    printf("Enter text (Ctrl+C to quit):\n");

    char buffer[MAX_LINE_LEN];
    while (1) {
        if (fgets(buffer, MAX_LINE_LEN, stdin) != NULL) {
            if (line_count < MAX_LINES) {
                lines[line_count] = strdup(buffer);
                line_count++;
            } else {
                printf("Reached maximum number of lines.\n");
            }
        }
    }
    return 0;
}

