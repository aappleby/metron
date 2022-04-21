#include <stdio.h>

#include "metron/uart_top.h"

int main(int argc, char** arv) {
  printf("Metron simulation:\n");
  printf("================================================================================\n");

  const int cycles_per_bit = 3;
  uart_top<cycles_per_bit> top;
  top.tock(0);

  for (int cycle = 0; cycle < 20000; cycle++) {
    bool old_valid = top.valid();
    top.tock(1);
    if (!old_valid && top.valid()) printf("%c", (uint8_t)top.data());
    
    if (top.done()) {
      printf("\n");
      printf("================================================================================\n");
      printf("%d\n", cycle);
      if (top.sum() == 0x0000b764) {
        printf("All tests pass.\n");
        return 0;
      }
    }
  }
  return -1;
}
