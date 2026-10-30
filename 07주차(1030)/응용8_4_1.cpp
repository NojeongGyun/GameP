#include <stdio.h>
#include <conio.h>
#include <windows.h>

void gotoxy(int x, int y);
void display_text(int count);
void display_question(void);
void display_result(void);
void intro_game(void);

char sol[10][11];  // 사용자의 입력 값을 저장할 배열 (최대 10개)

int main(void)
{
    intro_game();
    display_question();
    printf("\n문제풀이가 모두 끝났습니다.\n");
    printf("\n\n풀이를 보려면 아무키나 누르시오.");
    getch();
    display_result();
    return 0;
}

void gotoxy(int x, int y)
{
    COORD Pos = { (SHORT)(x - 1), (SHORT)(y - 1) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void display_text(int count)
{
    gotoxy(1, 2);
    printf("다음 문제를 읽고 답을 입력하시오.\n\n");
    printf("(%d번 문제) \n\n", count + 1);
}

void display_question(void)
{
    FILE *fp1;
    char chr1[82];
    int line = 5, count = 0;
    const char *file1 = "심리테스트질문.txt";

    fp1 = fopen(file1, "r");
    // [오류 수정 1] 파일 읽기 실패 예외 처리
    if (fp1 == NULL)
    {
        system("cls");
        printf("오류: '%s' 파일을 열 수 없습니다.\n", file1);
        printf("파일이 프로그램과 같은 폴더에 있는지 확인하세요.\n");
        getch();
        return;
    }

    system("cls");
    // [오류 수정 2] feof 대신 fgets의 반환값 체크
    while (fgets(chr1, sizeof(chr1), fp1) != NULL)
    {
        if (chr1[0] == '\n' || chr1[0] == 10)
        {
            // [오류 수정 3] 배열 범위를 넘어서지 않는지 확인 (최대 10개)
            if (count < 10)
            {
                display_text(count);
                gotoxy(1, line);
                scanf("%10s", sol[count]); // 입력 길이 제한
                system("cls");
                line = 5;
                count++;
            }
        }
        else
        {
            gotoxy(1, line++);
            printf("%s", chr1);
        }
    }

    // 마지막 문제 처리 (파일 끝에 줄바꿈이 없었을 경우)
    if (line > 5 && count < 10)
    {
        display_text(count);
        gotoxy(1, line);
        scanf("%10s", sol[count]);
    }

    fclose(fp1);
}

void display_result(void)
{
    FILE *fp2;
    char chr1[82];
    int line = 5, count = 0;
    const char *file2 = "심리테스트해설.txt";

    fp2 = fopen(file2, "r");
    // [오류 수정 1] 파일 읽기 실패 예외 처리
    if (fp2 == NULL)
    {
        system("cls");
        printf("오류: '%s' 파일을 열 수 없습니다.\n", file2);
        getch();
        return;
    }

    system("cls");
    // [오류 수정 2] feof 대신 fgets의 반환값 체크
    while (fgets(chr1, sizeof(chr1), fp2) != NULL)
    {
        if (chr1[0] == '\n' || chr1[0] == 10)
        {
            // [오류 수정 3] 배열 범위 체크
            if (count < 10)
            {
                gotoxy(1, line + 1);
                printf("당신의 답은 : %s\n", sol[count]);
                printf("다음문제의 해설은 아무키나 누르시오.");
                getch();
                system("cls");
                line = 5;
                count++;
            }
        }
        else
        {
            gotoxy(1, line++);
            printf("%s", chr1);
        }
    }

    // 마지막 해설 출력
    if (line > 5 && count < 10)
    {
        gotoxy(1, line + 1);
        printf("당신의 답은 : %s\n", sol[count]);
        printf("\n모든 해설이 끝났습니다. 아무키나 누르시오.");
        getch();
    }

    fclose(fp2);
}

void intro_game(void)
{
    system("cls");
    printf("심리테스트 게임\n\n");
    printf("제시되는 문제에 모두 답을 하고나면\n");
    printf("해설이 출력됩니다.\n\n");
    printf("아무키나 누르면 시작합니다. ");
    getch();
}
