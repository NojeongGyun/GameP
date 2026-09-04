<mark>커서 좌표 이동 함수</mark>
gotoxy(x,y)는 커서 좌표 이동 함수로 conio.h 헤더 안에 함수가 정의 되어있기 때문에, #include <conio.h>를 표기 후 사용하여야 합니다.
만약 conio.h를 사용하지 않고 gotoxy를 사용하려면 사용자가 직접 gotoxy함수를 정의 후 사용하면 됩니다.

<mark>화면 지우기</mark>
화면에 보이는 택스트를 지우려면 cls를 사용해야 합니다.
하지만 cls는 c언어의 함수가 아니라, Windows 명령이기 때문에 C언어로 작성 할 때에는 system("cls"); 로 작성하여야 합니다.

