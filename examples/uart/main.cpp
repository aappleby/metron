#include <stdio.h>

#include "metron/uart_top.h"

int main(int argc, char** arv) {
  printf("Metron simulation:\n");
  printf("================================================================================\n");

  const int cycles_per_bit = 3;
  uart_top<cycles_per_bit> top;
  top.tock(0);

  for (int cycle = 0; cycle < 20000; cycle++) {
    bool old_valid = top.tock_valid();
    top.tock(1);
    if (!old_valid && top.tock_valid()) printf("%c", (uint8_t)top.tock_data());
    
    if (top.tock_done()) {
      printf("\n");
      printf("================================================================================\n");
      printf("%d\n", cycle);
      if (top.tock_sum() == 0x0000b764) {
        printf("All tests pass.\n");
        return 0;
      }
    }
  }
  return -1;
}
