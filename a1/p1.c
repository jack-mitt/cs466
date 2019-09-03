#include <string.h>
#include <stdio.h>

int main(){
  char s[] = "ATTACK AT DAWN";
  int i;
  
  for(i = 0; i < strlen(s); i++){
    int b = s[i] ^ 0x42;
    printf("%d \n", b);
  }
  
  return 0;
}
