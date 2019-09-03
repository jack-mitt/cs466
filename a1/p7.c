#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char* argv[]){
  int zero_count = 0;
  int one_count = 0;
  char c;
  if(!argv[1]){
    printf("ERROR: no argument.\n");
    exit(1);
  }
  FILE *fptr = fopen(argv[1], "r");
  if (fptr == NULL){
    printf("ERROR: can't read. \n");
    exit(1);
  }

  int mask;
  while((c = fgetc(fptr)) != EOF){
    // 10000000, moves the one down one by one extracting one bit at a time
    mask = 0x80;
    while(mask > 0){
      if ((c & mask) > 0){
        one_count++;
      } else {
        zero_count++;
      }
      mask >>= 1;
    }
  }

  printf("O: %d \n", one_count);
  printf("1: %d \n", zero_count);
  fclose(fptr);
  return 0;
}
