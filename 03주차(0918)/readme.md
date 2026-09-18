<pre>
[ <mark>문자형 숫자의 아스키코드</mark> ]
  int sub_menu_display01(void)
  {
  int select;
  system("cls");
  printf("햄버거 만들기\n\n");
  printf("1. 치킨버거\n");
  printf("2. 치즈버거\n");
  printf("3. 메인 메뉴로 이동\n\n");
  printf("메뉴번호 입력>");
  select=getch()-48;
  return select;
  } 
  ㄴ> 코드안의 getch()-48; 에서 getch의 반환형은 int형입니다. 그렇기 때문에 입력받을 때는 문자형으로 입력받고, 반환할 때는 문자의 
      아스키코드의 숫자값으로 반환이 되게 됩니다. 문자 0의 아스키코드 값은 48, 문자 1의 아스키코드 값은 49,.., 문자 9의 아스키 코드 
      값은 57 입니다. 이 아스키 코드를 이용해 문자로 된 두자리 숫자 혹은 그 이상 자리의 숫자를 만들수 있습니다. 예를 들어 19라는 
      문자를 표현하고 싶다라고 하면 printf("%c", 49); printf("%c", 57); 를 하면 화면에 19라는 문자가 화면에 보여지게 됩니다.
      위에 있는 해당 코드는 어떤 메뉴의 숫자를 선택했냐라는 질의의 값이라고 할 수 있습니다.
      
[ <mark>난수 생성</mark> ]
  #include <stdio.h>
  #include <stdlib.h>
  #include <time.h>
  int main(void)
  {
  int i;
  srand(time(NULL));
  for(i=1;i<=6;i++)
  printf("%2d:%d\n",rand()%45+1);
  return 0;
  }
  ㄴ> 이 코드는 1~45까지 하나의 숫자를 출력하는 프로그램입니다. 위 코드에서 srand가 보이는데, 이는 seed random의 줄임말이고, 
      정확히는 rand()가 만들어낼 난수열의 시작값(seed)을 설정하는 함수입니다. 컴퓨터는 난수를 생성 할 때 컴퓨터의 규칙대로 생성하기
      때문에 시작값을 하나의 특정 값으로 고정 시킨 후 실행을 여러번 한다면 같은 규칙이 적용되어 매번 똑같은 난수가 생성 될 것 입니다.
      이를 방지하기 위해 실행 할 때 마다 매번 바뀌는 seed값을 설정을 해야 실행마다 난수가 다르게 생성 될 것입니다. 난수를 다르게 생성하기 
      위해 time(null)이라는 실행하는 현재 시간에 기준해서 seed를 time의 숫자형으로 반환시켜 난수를 매번 다르게 생성 할 수 있습니다. 
      
[ <mark>문자 배열 </mark> ]
struct trump
{
    char order;
    char shape[3];
    char number;
};

trump card[52];

int i, j;

char shape[4][3] = {"♠", "♦", "♥", "♣"};

for (i = 0; i < 4; i++)
{
    for (j = i * 13; j < i * 13 + 13; j++)
    {
        m_card[j].order = i;

        strcpy(m_card[j].shape, shape[i]);

        m_card[j].number = j % 13 + 1;

        switch (m_card[j].number)
        {
            // 1일 경우 number에 'A' 저장
            // 11일 경우 number에 'J' 저장
            // 12일 경우 number에 'Q' 저장
            // 13일 경우 number에 'K' 저장
        }
    }
}
ㄴ-> 이 코드는 스페이드, 다이아, 하트, 클로버 중 1개 그리고 숫자중 1개를 임의로 입력받아 화면에 출력하는 프로그램입니다.  
     이 코드 안에 char shape[3]가 있습니다. 배열 크기를 3을 잡는 이유는 문양, 숫자를 받고, 문자열 끝을 나타내는 /0이
     넣어지기 때문에 char shape[2]가 아닌 char shape[3]으로 코드를 작성한 이유입니다.
    
</pre>
