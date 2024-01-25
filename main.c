#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

struct Snake {
    int x, y;
};

struct Food {
    int x, y;
};


// 判断是否吃到food
bool eat_food(struct Snake snake[], struct Food food[]);
// 判断是否撞墙
bool collision(struct Snake snake[], int rows, int cols);
// 判断是否吃到自己
bool eat_self(struct Snake snake[], int length);

// 生成food
void generate_food(struct Food food[], int rows);

// 绘制snake
void draw_snake(WINDOW *win, struct Snake snake[], int length, int directionY, int directionX);


int main() {
    initscr();
    raw();
    noecho();
    curs_set(0);
    start_color();

    // 设置颜色配色
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    attr_t emphasis = A_REVERSE | COLOR_PAIR(1);


    // 确定窗口位置
    int y_max, x_max, rows, cols, begin_y, begin_x;
    int score = 0;
    char str[25];
    struct Snake snake[50] = {{2, 2},
                              {1, 2}};

    int length = 2;
    struct Food food[] = {5, 5};
    int pressed;
    int directionX = 0;
    int directionY = 0;

    getmaxyx(stdscr, y_max, x_max);
    rows = y_max / 2;
    cols = x_max / 2;
    begin_y = y_max / 4;
    begin_x = x_max / 4;
    WINDOW *win = newwin(rows, cols, begin_y, begin_x);
    keypad(win, true);
    nodelay(win, true);
    refresh();

    while ((pressed = wgetch(win)) != 27 && !collision(snake, rows, cols) && !eat_self(snake, length)) {
        switch (pressed) {
            case KEY_LEFT:
                // 注意因为directionX和directionY是全局变量，状态会保留
                if (directionX != 1) {
                    directionX = -1;
                    directionY = 0;
                }
                break;
            case KEY_RIGHT:
                if (directionX != -1) {
                    directionX = 1;
                    directionY = 0;
                }
                break;
            case KEY_UP:
                if (directionY != 1) {
                    directionY = -1;
                    directionX = 0;
                }
                break;
            case KEY_DOWN:
                if (directionY != -1) {
                    directionY = 1;
                    directionX = 0;
                }
                break;
            default:
                break;
        }

        wclear(win);
        wborder(win, '+', '+', '-', '-', '-', '-', '-', '-');
        // 绘制snake
        draw_snake(win, snake, length, directionY, directionX);
        // 绘制食物

        wattron(win, A_REVERSE | COLOR_PAIR(2));
        mvwprintw(win, food[0].y, food[0].x, "o");
        wattroff(win, A_REVERSE | COLOR_PAIR(2));
        // 绘制分数
        sprintf(str, "Score: %d", score);
        wattron(win, emphasis);
        mvwaddstr(win, 0, begin_x / 1.3, str);
        wattroff(win, emphasis);
        if (eat_food(snake, food)) {
            length++;
            snake[length].x = food[0].x;
            snake[length].y = food[0].y;
            score++;
            generate_food(food, rows);
        }
        usleep(200000);
    }

    wrefresh(win);
    endwin();
    return 0;
}


void draw_snake(WINDOW *win, struct Snake snake[], int length, int directionY, int directionX) {
    for (int i = length - 1; i > 0; i--) {
        // 让每一截蛇身的位置等于他的前一截蛇身的位置
        snake[i].x = snake[i - 1].x;
        snake[i].y = snake[i - 1].y;
    }

    // 将蛇头，按照贪吃蛇的移动方向增加一个格子
    snake[0].x += directionX;
    snake[0].y += directionY;

    for (int i = 0; i < length; ++i) {
        mvwaddstr(win, snake[i].y, snake[i].x, "*");
    }

}

bool eat_food(struct Snake snake[], struct Food food[]) {
    return (snake[0].x == food[0].x && snake[0].y == food[0].y) != 0;
}

void generate_food(struct Food food[], int rows) {
    srand(time(NULL));
    food[0].x = rand() % (rows - 2) + 1;
    food[0].y = rand() % (rows - 2) + 1;
}

bool collision(struct Snake snake[], int rows, int cols) {
    return (snake[0].x < 0 || snake[0].y < 0 || snake[0].x > cols || snake[0].y > rows) != 0;
}

bool eat_self(struct Snake snake[], int length) {
    int x = snake[0].x;
    int y = snake[0].y;
    for (int i = 2; i < length; ++i) {
        if (x == snake[i].x && y == snake[i].y) {
            return true;
        }
    }
    return false;
}