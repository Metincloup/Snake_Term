#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#define cols 32
#define rows 32
#define right 0
#define left 1
#define up 2
#define down 3

char Map[cols][rows] = {0};
int score = 0;
short appleX, appleY;
short state = 1;

typedef struct Snake {
    short x, y;
    struct Snake* next;
} Snake;

char getChar() {
    char c;
    struct termios oldt, newt;

    // Get the current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);

    // Copy the settings to a new struct
    newt = oldt;

    // Set the new terminal settings
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Read a character
    c = getchar();

    // Restore the original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return c;
}

void Render() {
    system("clear");

    printf("score: %d\n\n\n", score);
    for (size_t i = 0; i < cols; i++) {
        for (size_t j = 0; j < rows; j++) {
            if (Map[i][j] == NULL)
                Map[i][j] = ' ';
            printf("%c", Map[i][j]);
        }
        printf("\n");
    }
    printf("\n\n\n\n\n");
}

void resetMap() {
    for (size_t i = 0; i < cols; i++) {
        for (size_t j = 0; j < rows; j++) {
            Map[i][j] = ' ';
        }
    }
}

void addTail(Snake* snake) {
    Snake* tempSnake = snake;

    while (tempSnake->next != NULL) {
        tempSnake = tempSnake->next;
    }

    Snake* newSegment = (Snake*)malloc(sizeof(Snake));  // Allocate memory for the new segment
    newSegment->x = tempSnake->x;
    newSegment->y = tempSnake->y;
    newSegment->next = NULL;

    tempSnake->next = newSegment;
}

void MoveSnake(Snake* snake, short rotation) {
    short oldx = snake->x;
    short oldy = snake->y;

    switch (rotation) {
        case right:
            snake->x++;
            break;
        case left:
            snake->x--;
            break;
        case up:
            snake->y--;
            break;
        case down:
            snake->y++;
            break;
    }

    if (snake->x >= rows)
        snake->x = 0;
    else if (snake->x < 0)
        snake->x = rows - 1;

    if (snake->y >= cols)
        snake->y = 0;
    else if (snake->y < 0)
        snake->y = cols - 1;

    if (Map[snake->y][snake->x] == '#') {
        dead();
    }

    Map[snake->y][snake->x] = '#';

    if (snake->x == appleX && snake->y == appleY) {
        addTail(snake);
        score++;
        appleGen();
    } else {
        Map[oldy][oldx] = ' ';
        Snake* tempSnake = snake->next;
        while (tempSnake != NULL) {
            short tempx = tempSnake->x;
            short tempy = tempSnake->y;
            tempSnake->x = oldx;
            tempSnake->y = oldy;
            oldx = tempx;
            oldy = tempy;
            Map[tempSnake->y][tempSnake->x] = '#';
            tempSnake = tempSnake->next;
        }
        Map[oldy][oldx] = ' ';
    }
}

void applyMap(Snake* snake) {
    Snake* tempSnake = snake;

    while (tempSnake != NULL) {
        Map[tempSnake->y][tempSnake->x] = '#';
        tempSnake = tempSnake->next;
    }

    Map[appleY][appleX] = 'O';
}

void appleGen() {
    appleX = rand() % rows;
    appleY = rand() % cols;
}

void dead() {
    system("clear");
    printf("\n\n\n\n\n\n You are Dead \n\n\n\n\n Your SCORE is %d", score);
    state = 0;
}

void Tick(Snake* snake) {
    char rotation = getChar();
    resetMap();
    applyMap(snake);
    Render();

    if (rotation == 'a')
        MoveSnake(snake, left);
    if (rotation == 's')
        MoveSnake(snake, down);
    if (rotation == 'w')
        MoveSnake(snake, up);
    if (rotation == 'd')
        MoveSnake(snake, right);
}

int main(void) {
    srand(time(0));

    Snake* snake = (Snake*)malloc(sizeof(Snake));
    snake->x = rand() % rows;
    snake->y = rand() % cols;
    snake->next = NULL;

    appleGen();

    resetMap();
    applyMap(snake);
    Render();

    long tempTime = clock();
    int tickCount = 0;
    double tickDuration = 1.0 / 20.0;  // 20 ticks per second

    while (state) {
        long currentTime = clock();
        double elapsedTime = (double)(currentTime - tempTime) / CLOCKS_PER_SEC;

        if (elapsedTime >= tickDuration) {
            Tick(snake);
            tempTime = currentTime;
            tickCount++;
        }
    }

    return 0;
}
