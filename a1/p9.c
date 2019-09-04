#include <stdio.h>
#include <stdlib.h>
void foo(){
  int i = 0;
  while(i < 10){
    printf("%d ", i);
    i++;
  }
  printf("\n");
}

int main(int argc, char* argv[]){
  foo();
}
