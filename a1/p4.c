#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
bool xor_bit(bool a, bool b){
  //xor 2 bits without using ^
  bool and_1 = (a && !b);
  bool and_2 = (!a && b);
  bool xor = and_1 || and_2;
  return xor;
}

uint32_t xor_word(uint32_t a, uint32_t b){
  //xor two words, cakk xor_bit for every bit
  int c_bit = 0;
  bool xor_check;
  int byte;
  int mask;
  uint32_t c = 0x0;
  while(c_bit < 32){
    xor_check = xor_bit(a & 1, b & 1);
    if(xor_check){
      byte = 1;
    } else {
      byte = 0;
    }
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
