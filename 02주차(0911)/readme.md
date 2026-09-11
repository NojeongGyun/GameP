<pre>
[ 버퍼 처리 ]
#include <stdio.h>int main(){        
  char string[20];        
  char c;        
  scanf("%s", string);        
  scanf("%c", &c);        
  printf("%s\n", string);        
  printf("!!%c!!\n", c);        
  return 0;}
  ㄴ-> Scanf를 사용하면 입력 받은 "string"으로 정의된 문자열은 버퍼에 임시 저장되고 printf로 출력되게 됩니다. 
       하지만 'c'로 정의된 문자형 변수는 Sacnf를 문자열이 먼저 받고 다음 문자 'c'를 Scanf를 입력받기 위해 개행문자(\n)를 
       입력하게 되고, 버퍼에 개행문자가 남게되어 'c' 변수로 입력되게 되어 해당 코드는 문자열과 문자 변수안에 있는 개행문자를 적용하여 화면에 보이게 됩니다.
       이를 고치는 해결법으로 한 문자만 입력받는 함수 getchar(); 를 scanf사이에 추가를 하여 입력버퍼를 초기화 하여 원하는 문자열과 문자를 출력할 수 있습니다.
  
      




  
</pre>

