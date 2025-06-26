#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_LEN 300

typedef struct {
    int x, y;
} Position;

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

Position snake[MAX_LEN];
int snake_length = 5;
Direction dir = RIGHT;
Position food;
int game_over = 0;
int score = 0;
int paused = 0;
volatile sig_atomic_t sigint_flag = 0;

void init_game(){
    initscr();
    noecho();
    curs_set(0);
    timeout(100);
    srand(time(NULL));
    for(int i = 0; i < snake_length; i++){
        snake[i].x = WIDTH / 2 - i;
        snake[i].y = HEIGHT / 2;
    }
    food.x = rand() % (WIDTH - 2) + 1;
    food.y = rand() % (HEIGHT - 2) + 1;
}

void end_game(){
    endwin();
    printf("Game Over! Final Score: %d\n", score);
}

void draw_border(){
    for(int i = 0; i < WIDTH; i++){
        mvaddch(0, i, '#');
        mvaddch(HEIGHT - 1, i, '#');
    }
    for(int i = 0; i < HEIGHT; i++){
        mvaddch(i, 0, '#');
        mvaddch(i, WIDTH - 1, '#');
    }
}

void draw_snake(){
    for(int i = 0; i < snake_length; i++){
        mvaddch(snake[i].y, snake[i].x, 'O');
    }
}

void draw_food(){
    mvaddch(food.y, food.x, '@');
}

void draw_status(){
    mvprintw(HEIGHT, 0, "Score: %d, Length: %d", score, snake_length);
}

void move_snake(){
    for(int i = snake_length - 1; i > 0; i--){
        snake[i] = snake[i-1];
    }
    switch(dir){
        case UP:    snake[0].y--; break;
        case DOWN:  snake[0].y++; break;
        case LEFT:  snake[0].x--; break;
        case RIGHT: snake[0].x++; break;
    }
    if(snake[0].x <= 0 || snake[0].x >= WIDTH - 1 ||
       snake[0].y <= 0 || snake[0].y >= HEIGHT - 1){
        game_over = 1;
        return;
    }
    for(int i = 1; i < snake_length; i++){
        if(snake[0].x == snake[i].x && snake[0].y == snake[i].y){
            game_over = 1;
            return;
        }
    }
    if(snake[0].x == food.x && snake[0].y == food.y){
        if(snake_length < MAX_LEN)
            snake_length++;
        score++;
        food.x = rand() % (WIDTH - 2) + 1;
        food.y = rand() % (HEIGHT - 2) + 1;
    }
}

void handle_input(){
    int ch = getch();
    switch(ch){
        case 'w':
            if(dir != DOWN) dir = UP;
            break;
        case 's':
            if(dir != UP) dir = DOWN;
            break;
        case 'a':
            if(dir != RIGHT) dir = LEFT;
            break;
        case 'd':
            if(dir != LEFT) dir = RIGHT;
            break;
        case 'p':
        case 'P':
            paused = !paused;
            break;
    }
}

void handle_sigint(int sig){
    (void)sig;
    sigint_flag = 1;
}

void handle_sigtstp(int sig){
    (void)sig;
    paused = !paused;
}

int main(){
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);
    init_game();
    while(!game_over){
        clear();
        draw_border();
        draw_food();
        draw_snake();
        draw_status();
        if(sigint_flag){
            nodelay(stdscr, FALSE);
            mvprintw(HEIGHT+1, 0, "Are you sure you want to quit? (y/n): ");
            refresh();
            int c = getch();
            if(c == 'y' || c == 'Y'){
                endwin();
                printf("Terminated by user. Final Score: %d\n", score);
                exit(0);
            } else {
                sigint_flag = 0;
                nodelay(stdscr, TRUE);
            }
        }
        if(paused){
            mvprintw(HEIGHT/2, (WIDTH - strlen("== PAUSED ==")) / 2, "== PAUSED ==");
            mvprintw(HEIGHT/2 + 1, (WIDTH - strlen("Press 'p' or Ctrl+Z to resume")) / 2, "Press 'p' or Ctrl+Z to resume");
            refresh();
            int c = getch();
            if(c == 'p' || c == 'P')
                paused = !paused;
            continue;
        }
        handle_input();
        move_snake();
        refresh();
        usleep(100000);
    }
    end_game();
    return 0;
}

