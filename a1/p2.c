#include <stdio.h>
#include <stdlib.h>
int main(){
  float *array;
  array = (float*)malloc(10 * sizeof(float));
  int i;
  float count = 1.0;
  float sum = 0.0;
  for(i = 0; i < 10; i++){
    array[i] = count;
    sum += count;
    count++;
  }
  printf("%f \n", sum);
  
  return 0;
}
