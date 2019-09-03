#include <stdio.h>
#include <stdint.h>
int main(){
  uint32_t a = 0xaabbccdd;
  uint32_t b = 0x11223344;
  uint32_t a_evens = a & 0xAAAAAAAA;
  b = b & 0x55555555;
  b = b | a_evens;
  printf("%x\n", b);
  return 0;
}
