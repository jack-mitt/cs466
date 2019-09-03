#include <stdio.h>
#include <stdint.h>
union Convert {
  uint32_t k;
  float flt;
};

int main(){
  float f = 42.0;
  union Convert convert;
  convert.flt = f;
  printf("%d \n", convert.k);
}
