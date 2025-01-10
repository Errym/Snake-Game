#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>   // For timeval
#include <sys/select.h> // For fd_set

#define WIDTH 40
#define HEIGHT 20

typedef struct {
    int x, y;
} Point;

Point food, snake[100];
int length = 1;
char direction = 'd'; // Initial direction (right)

// Non-blocking input setup
void enableRawMode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void disableRawMode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Check if key was pressed
int kbhit() {
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}

// Draw the game board
void draw() {
    system("clear");
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1) {
                printf("#");
            } else if (x == food.x && y == food.y) {
                printf("*");
            } else {
                int isSnake = 0;
                for (int i = 0; i < length; i++) {
                    if (snake[i].x == x && snake[i].y == y) {
                        isSnake = 1;
                        break;
                    }
                }
                printf(isSnake ? "O" : " ");
            }
        }
        printf("\n");
    }
}

// Update the game state
void update() {
    Point next = snake[0];
    if (kbhit()) {
        char newDir = getchar();
        if ((newDir == 'w' || newDir == 'a' || newDir == 's' || newDir == 'd') &&
            abs(newDir - direction) != 2) {
            direction = newDir;
        }
    }
    switch (direction) {
        case 'w': next.y--; break;
        case 's': next.y++; break;
        case 'a': next.x--; break;
        case 'd': next.x++; break;
    }

    for (int i = length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    snake[0] = next;

    // Check collisions
    if (next.x == 0 || next.x == WIDTH - 1 || next.y == 0 || next.y == HEIGHT - 1) {
        printf("Game Over! You hit the wall.\n");
        exit(0);
    }
    for (int i = 1; i < length; i++) {
        if (snake[i].x == next.x && snake[i].y == next.y) {
            printf("Game Over! You hit yourself.\n");
            exit(0);
        }
    }

    // Check food
    if (next.x == food.x && next.y == food.y) {
        length++;
        food.x = rand() % (WIDTH - 2) + 1;
        food.y = rand() % (HEIGHT - 2) + 1;
    }
}

int main() {
    srand(time(NULL));
    food.x = rand() % (WIDTH - 2) + 1;
    food.y = rand() % (HEIGHT - 2) + 1;
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;

    enableRawMode();

    while (1) {
        draw();
        update();
        usleep(100000); // Slow down game loop
    }

    disableRawMode();
    return 0;
}
