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
  ㄴ-> Scanf를 사용하면 입력 받은 "string"으로 정의된 문자열은 버퍼에 저장되고 printf로 출력되게 됩니다. 
       하지만 'c'로 정의된 문자형 변수는 Sacnf를 문자열이 먼저 받고 다음 문자 'c'를 Scanf를 입력받기 위해 개행문자(\n)를 
       입력하게 되고, 개행문자가 'c' 변수로 입력되게 되어 해당 코드는 문자열만 print 되게 됩니다.
      
  
</pre>

