#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

static const char *s_board =
"+ - + - + - + - + - + - + - + - + \n"
"|   |   |   | > | / |   |   |   | \n"
"+ - + - + - + - * - + - + - + - + \n"
"|   |   |   | / | > |   |   |   | \n"
"+ - # - + - + - + - + - + - # - + \n"
"|   |   |   |   |   |   |   |   | \n"
"# - + - # - + - # - + - # - + - # \n"
"|   |   |   |   |   |   |   |   | \n"
"+ - + - + - + - + - + - + - + - + \n"
"|    楚   河         汉   界    | \n"
"+ - + - + - + - + - + - + - + - + \n"
"|   |   |   |   |   |   |   |   | \n"
"# - + - # - + - # - + - # - + - # \n"
"|   |   |   |   |   |   |   |   | \n"
"+ - # - + - + - + - + - + - # - + \n"
"|   |   |   | > | / |   |   |   | \n"
"+ - + - + - + - * - + - + - + - + \n"
"|   |   |   | / | > |   |   |   | \n"
"+ - + - + - + - + - + - + - + - + \n\n"
"E-上 D-下 S-左 F-右 空格-选子/走子\n"
"C-取消选子  B-悔棋  R-新局  Q-退出\n";

static const char s_qiju_init[10][9] = {
    { 'c', 'm', 'x', 's', 'j', 's', 'x', 'm', 'c' },
    {  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  },
    {  0 , 'p',  0 ,  0 ,  0 ,  0 ,  0 , 'p',  0  },
    { 'b',  0 , 'b',  0 , 'b',  0 , 'b',  0 , 'b' },
    {  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  },
    {  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  },
    { 'B',  0 , 'B',  0 , 'B',  0 , 'B',  0 , 'B' },
    {  0 , 'P',  0 ,  0 ,  0 ,  0 ,  0 , 'P',  0  },
    {  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0  },
    { 'C', 'M', 'X', 'S', 'J', 'S', 'X', 'M', 'C' },
};

static const char *get_qizi_name(char c)
{
    switch (c) {
    case 'c': return "車";
    case 'C': return "俥";
    case 'm': return "馬";
    case 'M': return "傌";
    case 'x': return "象";
    case 'X': return "相";
    case 's': return "士";
    case 'S': return "仕";
    case 'j': return "将";
    case 'J': return "帥";
    case 'p': return "砲";
    case 'P': return "炮";
    case 'b': return "卒";
    case 'B': return "兵";
    default : return "  ";
    }
}

static void draw_game(char qiju[10][9], int qiziselx, int qizisely, int cursorx, int cursory, int turns, char eat[2][17])
{
    int x = 0, y = 0, i = 0, row, col, color;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD  c = {0};
    SetConsoleCursorPosition(h, c);
    printf("         中国象棋 v1.0.0\n\n");
    while (s_board[i]) {
        row = y / 2; col = x / 4;
        color = x % 4 == 0 && y % 2 == 0 && row == cursory && col == cursorx ? BACKGROUND_INTENSITY | BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE : 0;
        if (x++ % 4 == 0 && y % 2 == 0 && qiju[row][col] && row <= 9) {
            color |= qiju[row][col] >= 'a' && qiju[row][col] <= 'z' ? FOREGROUND_GREEN : FOREGROUND_RED;
            color |= row == qizisely && col == qiziselx ? 0 : FOREGROUND_INTENSITY;
            SetConsoleTextAttribute(h, color);
            printf(get_qizi_name(qiju[row][col]));
            x++; i++;
        } else {
            SetConsoleTextAttribute(h, color|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
            putchar(s_board[i] == '>' ? '\\' : s_board[i]);
        }
        if (s_board[i++] == '\n') { x = 0; y++; }
    }
    c.X = 35; c.Y =  3; SetConsoleCursorPosition(h, c);
    printf("%s 黑方", (turns & 1) ? "@" : " ");
    c.X = 35; c.Y = 11; SetConsoleCursorPosition(h, c);
    printf("第 %d 轮", turns / 2 + 1);
    c.X = 35; c.Y = 13; SetConsoleCursorPosition(h, c);
    printf("%s 红方", (turns & 1) ? " " : "@");
    for (row=0; row<4; row++) {
        c.X = 35; c.Y = 5 + row; SetConsoleCursorPosition(h, c);
        SetConsoleTextAttribute(h, FOREGROUND_INTENSITY|FOREGROUND_RED);
        for (col=0; col<4; col++) printf("%s", get_qizi_name(eat[1][1+row*4+col]));
        c.X = 35; c.Y = 15+ row; SetConsoleCursorPosition(h, c);
        SetConsoleTextAttribute(h, FOREGROUND_INTENSITY|FOREGROUND_GREEN);
        for (col=0; col<4; col++) printf("%s", get_qizi_name(eat[0][1+row*4+col]));
    }
    SetConsoleTextAttribute(h, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
    c.X =  0; c.Y = 24; SetConsoleCursorPosition(h, c);
}

static void limit_cursorxy(int *x, int *y)
{
    if (*x < 0) *x = 0;
    if (*x > 8) *x = 8;
    if (*y < 0) *y = 0;
    if (*y > 9) *y = 9;
}

static int check_pick(char qiju[10][9], int curx, int cury, int turn)
{
    int type = qiju[cury][curx] >= 'a' && qiju[cury][curx] <= 'z';
    return (type == turn) && qiju[cury][curx];
}

static int check_move(char qiju[10][9], int srcx, int srcy, int dstx, int dsty)
{
    int dx   = abs(srcx - dstx);
    int dy   = abs(srcy - dsty);
    int dist = dx + dy;
    int minx = srcx > dstx ? dstx : srcx;
    int maxx = srcx > dstx ? srcx : dstx;
    int miny = srcy > dsty ? dsty : srcy;
    int maxy = srcy > dsty ? srcy : dsty;
    int min, max, i, n;
    int srctype = qiju[srcy][srcx] >= 'a' && qiju[srcy][srcx] <= 'z';
    int dsttype = qiju[dsty][dstx] >= 'a' && qiju[dsty][dstx] <= 'z';
    if (qiju[dsty][dstx] && srctype == dsttype) return 0;
    switch (qiju[srcy][srcx]) {
    case 'b': return dist == 1 && dsty > srcy - (srcy >= 5);
    case 'B': return dist == 1 && dsty < srcy + (srcy <= 4);
    case 'j': return dstx >= 3 && dstx <= 5 && dsty <= 2 && dist == 1;
    case 'J': return dstx >= 3 && dstx <= 5 && dsty >= 7 && dist == 1;
    case 's': return dstx >= 3 && dstx <= 5 && dsty <= 2 && dist == 2 && dx == 1;
    case 'S': return dstx >= 3 && dstx <= 5 && dsty >= 7 && dist == 2 && dx == 1;
    case 'x': return dsty <= 4 && dx == 2 && dy == 2 && !qiju[miny + 1][minx + 1];
    case 'X': return dsty >= 5 && dx == 2 && dy == 2 && !qiju[miny + 1][minx + 1];
    case 'm': case 'M':
        if (dist == 3) {
            if (dx == 1 && !qiju[miny + 1][srcx]) return 1;
            if (dy == 1 && !qiju[srcy][minx + 1]) return 1;
        }
        return 0;
    case 'c': case 'C':
    case 'p': case 'P':
        if (srcx == dstx) {
            min = miny + 1; max = maxy - 1;
            for (n=0,i=min; n<2&&i<=max; i++) {
                if (qiju[i][dstx]) n++;
            }
        } else if (srcy == dsty) {
            min = minx + 1; max = maxx - 1;
            for (n=0,i=min; n<2&&i<=max; i++) {
                if (qiju[dsty][i]) n++;
            }
        } else return 0;
        switch (n) {
        case 0: return (qiju[srcy][srcx] == 'c' || qiju[srcy][srcx] == 'C') ||  !qiju[dsty][dstx];
        case 1: return (qiju[srcy][srcx] == 'p' || qiju[srcy][srcx] == 'P') && !!qiju[dsty][dstx];
        default:return 0;
        }
    }
    return 1;
}

static void do_move(char qiju[10][9], char srcx, char srcy, char dstx, char dsty)
{
    qiju[dsty][dstx] = qiju[srcy][srcx];
    qiju[srcy][srcx] = 0;
}

static void un_move(char qiju[10][9], char srcx, char srcy, char dstx, char dsty, char dstq)
{
    qiju[srcy][srcx] = qiju[dsty][dstx];
    qiju[dsty][dstx] = dstq;
}

static void record_move(char qipu[1024][5], int top, char srcx, char srcy, char dstx, char dsty, char dstq)
{
    if (top < 1024) {
        qipu[top][0] = srcx; qipu[top][1] = srcy;
        qipu[top][2] = dstx; qipu[top][3] = dsty;
        qipu[top][4] = dstq;
    }
}

int main(void)
{
    char qiju[10][9], eat[2][17] = {0}, qipu[1024][5];
    int  cursorx[2] = { 4, 4 }, cursory[2] = { 9, 0 };
    int  qizisel_x = -1, qizisel_y = -1;
    int  turn = 0, numt = 0, key = 0;
    memcpy(qiju, s_qiju_init, sizeof(qiju));

    while (key != 'q' && key != 'Q') {
        turn = numt & 1; draw_game(qiju, qizisel_x, qizisel_y, cursorx[turn], cursory[turn], numt, eat);
        switch ((key = getch())) {
        case 'e': case 'E': cursory[turn]--; break; // up
        case 'd': case 'D': cursory[turn]++; break; // down
        case 's': case 'S': cursorx[turn]--; break; // left
        case 'f': case 'F': cursorx[turn]++; break; // right
        case 'c': case 'C': qizisel_x = qizisel_y = -1; break;
        case 'r': case 'R':
            printf("重新开始吗？(Y/N) "); key = getch();
            if (key == 'y' || key == 'Y') {
                memcpy(qiju, s_qiju_init, sizeof(qiju));
                memset(eat, 0, sizeof(eat));
                cursorx[0] = 4; cursorx[1] = 4;
                cursory[0] = 9; cursory[1] = 0;
                turn = 0; numt = 0; qizisel_x = -1;
            } else system("cls");
            break;
        case ' ':
            if (qizisel_x == -1 && check_pick(qiju, cursorx[turn], cursory[turn], turn)) {
                qizisel_x = cursorx[turn];
                qizisel_y = cursory[turn];
            } else if (qizisel_x != -1 && check_move(qiju, qizisel_x, qizisel_y, cursorx[turn], cursory[turn])) {
                if (qiju[cursory[turn]][cursorx[turn]]) eat[turn][++eat[turn][0]] = qiju[cursory[turn]][cursorx[turn]];
                record_move(qipu, numt, qizisel_x, qizisel_y, cursorx[turn], cursory[turn], qiju[cursory[turn]][cursorx[turn]]);
                do_move(qiju, qizisel_x, qizisel_y, cursorx[turn], cursory[turn]);
                qizisel_x = -1; qizisel_y = -1;
                numt++;
            }
            break;
        case 'b': case 'B':
            if (numt > 0) {
                numt--; un_move(qiju, qipu[numt][0], qipu[numt][1], qipu[numt][2], qipu[numt][3], qipu[numt][4]);
                cursorx[numt&1] = qipu[numt][0]; cursory[numt&1] = qipu[numt][1];
                if (qipu[numt][4]) eat[numt&1][eat[numt&1][0]--] = 0;
            }
            if (numt > 0) {
                numt--; un_move(qiju, qipu[numt][0], qipu[numt][1], qipu[numt][2], qipu[numt][3], qipu[numt][4]);
                cursorx[numt&1] = qipu[numt][0]; cursory[numt&1] = qipu[numt][1];
                if (qipu[numt][4]) eat[numt&1][eat[numt&1][0]--] = 0;
            }
            qizisel_x = -1; qizisel_y = -1;
            break;
        }
        limit_cursorxy(cursorx + turn, cursory + turn);
    }
    return 0;
}