#include <stdio.h>
#include <stdint.h>
int xor_bit(int a, int b){
  //xor 2 bits without using ^
  int and_1 = (a && !b);
  int and_2 = (!a && b);
  int xor = and_1 || and_2;
  //printf("%d \n", xor);
  return xor;
}

uint32_t xor_word(uint32_t a, uint32_t b){
  //xor two words, cakk xor_bit for every bit
  int c_bit = 0;
  int byte;
  int mask;
  uint32_t c = 0x0;
  while(c_bit < 32){
    //printf("%01X ", (int)a & 1);
    //printf("%01X \n", (int)b & 1);
    byte = xor_bit((int)a & 1,(int) b & 1);
    mask = 1 << c_bit;
    c = c << (c & ~mask) | ((byte << c_bit) & mask);
    c_bit++;
    a = a >> 1;
    b = b >> 1;
  }
  return c;
}

int main(){
  uint32_t a = 0xaabbccdd;
  uint32_t b = 0x11223344;
  uint32_t c = xor_word(a, b);
  printf("%x\n", c);
  return 0;
}
