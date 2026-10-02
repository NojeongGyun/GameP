#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <windows.h>

int calc_frequency(int octave, int inx);
void practice_piano(void);

int main(void)
{
    printf("1부터 8까지 숫자 키를 누르면\n");
    printf("각 음의 소리가 출력됩니다.\n\n");
    printf("1:도 2:레 3:미 4:파 5:솔 6:라 7:시 8:도\n");
    printf("프로그램 종료는 Esc키 \n");
    practice_piano();
    return 0;
}


void practice_piano(void)
{
    int index[] = {0, 2, 4, 5, 7, 9, 11, 12};
    int freq[8], code, i;

    for (i = 0; i < 8; i++)
        freq[i] = calc_frequency(4, index[i]);

    do
    {
        code = getch();
        if ('1' <= code && code <= '8')
        {
            code -= 49; 
            Beep(freq[code], 300);
        }
    } while (code != 27); 
}

int calc_frequency(int octave, int inx)
{
  
    double freq = 16.35 * pow(2.0, octave) * pow(2.0, inx / 12.0);
    return (int)freq;
}
