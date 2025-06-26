#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_INPUT 1024
#define RED "\033[31m"
#define RESET "\033[0m"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"target sentence\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char *target = argv[1];
    int target_len = strlen(target);
    printf("Type the following sentence:\n%s\nStart typing: ", target);
    fflush(stdout);
    struct timeval start, end;
    gettimeofday(&start, NULL);
    struct termios orig_termios, raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    int correct = 0;
    int typed_count = 0;
    int idx = 0;
    char user_input[MAX_INPUT] = {0};
    while (idx < target_len) {
        char c;
        if (read(STDIN_FILENO, &c, 1) < 0) {
            break;
        }
        if (c == 127 || c == 8) {
            if (idx > 0) {
                idx--;
                typed_count--;
                if (user_input[idx] == target[idx]) {
                    correct--;
                }
                printf("\b \b");
                fflush(stdout);
            }
        } else {
            user_input[idx] = c;
            if (c == target[idx]) {
                write(STDOUT_FILENO, &c, 1);
                correct++;
            } else {
                printf(RED "%c" RESET, c);
            }
            fflush(stdout);
            idx++;
            typed_count++;
        }
    }
    gettimeofday(&end, NULL);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    double accuracy = 0.0;
    if (typed_count > 0) {
        accuracy = (double)correct / typed_count * 100.0;
    }
    double speed = typed_count / elapsed;
    printf("\n\n=== Result ===\n");
    printf("Time: %.2f seconds\n", elapsed);
    printf("Speed: %.2f chars/sec\n", speed);
    printf("Accuracy: %.2f%%\n", accuracy);
    return 0;
}

