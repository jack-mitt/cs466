#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char* argv[]){
  char contents;
  if(!argv[1]){
    printf("ERROR: no argument.\n");
    exit(1);
  }
  FILE *fptr = fopen(argv[1], "r");
  if (fptr == NULL){
    printf("ERROR: can't read. \n");
    exit(1);
  }
  
  contents = fgetc(fptr);
  while(contents != EOF){
    printf("%c", contents);
    contents = fgetc(fptr);
  }
  fclose(fptr);
  return 0;
}
