#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
int main(int argc, char* argv[]){
  struct stat buf;
  char str[80];
  int x;
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
  
  x = lstat(argv[1], &buf);
  if (S_ISLNK(buf.st_mode)){
    strcpy(str, "readlink ");
    strcat(str, argv[1]);
    system(str);
    printf("it is a link \n");
  }
  if (S_ISREG(buf.st_mode)){
    printf("ERROR: not a symlink\n");
  }
  
  return 0;  

}
