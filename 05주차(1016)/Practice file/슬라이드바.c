#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

#define WHITE_BG (BACKGROUND_RED | BACKGROUND_GREEN | \
                  BACKGROUND_BLUE | BACKGROUND_INTENSITY)

HANDLE console;

void gotoxy(int x, int y);
void draw_char(unsigned short code);
void draw_rectangle(int x, int y, int width, int height);
void draw_horizontal_slide(int x, int y, int length, int position);
void draw_vertical_slide(int x, int y, int length, int position);
void move_arrow_key(int key, int *x, int *y, int max_x, int max_y);

int main(void)
{
    int h_length, v_length;
    int x, y, key;
    WORD old_color;
    COORD size;
    CONSOLE_SCREEN_BUFFER_INFO info;
    CONSOLE_CURSOR_INFO old_cursor, cursor;

    x = 1;
    y = 1;

    console = GetStdHandle(STD_OUTPUT_HANDLE);

    setvbuf(stdout, NULL, _IONBF, 0);

    printf("수평 슬라이드바 길이(1~70): ");
    if (scanf("%d", &h_length) != 1)
        return 1;

    printf("수직 슬라이드바 길이(1~19): ");
    if (scanf("%d", &v_length) != 1)
        return 1;

    if (h_length < 1 || h_length > 70 ||
        v_length < 1 || v_length > 19)
    {
        printf("수평은 1~70, 수직은 1~19로 입력하세요.\n");
        return 1;
    }

    GetConsoleScreenBufferInfo(console, &info);
    old_color = info.wAttributes;

    size = info.dwSize;

    if (size.X < h_length + 18)
        size.X = (SHORT)(h_length + 18);

    if (size.Y < v_length + 10)
        size.Y = (SHORT)(v_length + 10);

    SetConsoleScreenBufferSize(console, size);

    GetConsoleCursorInfo(console, &old_cursor);

    cursor = old_cursor;
    cursor.bVisible = FALSE;
    SetConsoleCursorInfo(console, &cursor);

    SetConsoleTextAttribute(console, WHITE_BG);
    system("cls");

    gotoxy(3, 1);
    printf("Left/Right: Horizontal, Up/Down: Vertical, ESC: Exit");

    while (1)
    {
        draw_horizontal_slide(3, 5, h_length, x);
        draw_vertical_slide(h_length + 8, 5, v_length, y);

        key = getch();

        if (key == 27)
            break;

        if (key == 0 || key == 224)
        {
            key = getch();
            move_arrow_key(key, &x, &y, h_length, v_length);
        }
    }

    gotoxy(1, v_length + 9);

    SetConsoleTextAttribute(console, old_color);
    SetConsoleCursorInfo(console, &old_cursor);

    return 0;
}

void gotoxy(int x, int y)
{
    COORD pos;

    pos.X = (SHORT)(x - 1);
    pos.Y = (SHORT)(y - 1);

    SetConsoleCursorPosition(console, pos);
}

void draw_char(unsigned short code)
{
    WCHAR ch;
    DWORD written;

    ch = (WCHAR)code;
    WriteConsoleW(console, &ch, 1, &written, NULL);
}

void draw_rectangle(int x, int y, int width, int height)
{
    int i, j;

    SetConsoleTextAttribute(console, WHITE_BG);

    gotoxy(x, y);
    draw_char(0x250C);

    for (i = 0; i < width; i++)
        draw_char(0x2500);

    draw_char(0x2510);

    for (i = 0; i < height; i++)
    {
        gotoxy(x, y + i + 1);
        draw_char(0x2502);

        for (j = 0; j < width; j++)
            printf(" ");

        draw_char(0x2502);
    }

    gotoxy(x, y + height + 1);
    draw_char(0x2514);

    for (i = 0; i < width; i++)
        draw_char(0x2500);

    draw_char(0x2518);
}

void draw_horizontal_slide(int x, int y, int length, int position)
{
    draw_rectangle(x, y, length, 1);

    gotoxy(x + position, y + 1);
    SetConsoleTextAttribute(console, 0);
    printf(" ");

    SetConsoleTextAttribute(console, WHITE_BG);

    gotoxy(x + length - 1, y - 1);
    printf("%2d", position);
}

void draw_vertical_slide(int x, int y, int length, int position)
{
    draw_rectangle(x, y, 2, length);

    gotoxy(x + 1, y + position);
    SetConsoleTextAttribute(console, 0);
    printf("  ");

    SetConsoleTextAttribute(console, WHITE_BG);

    gotoxy(x + 5, y + length);
    printf("%2d", position);
}

void move_arrow_key(int key, int *x, int *y, int max_x, int max_y)
{
    switch (key)
    {
        case 72:
            if (*y > 1)
                (*y)--;
            break;

        case 80:
            if (*y < max_y)
                (*y)++;
            break;

        case 75:
            if (*x > 1)
                (*x)--;
            break;

        case 77:
            if (*x < max_x)
                (*x)++;
            break;
    }
}
