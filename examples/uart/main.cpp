#include <stdio.h>

#include "Platform.h"
#include "metron/uart_top.h"

void benchmark() {
  const int cycles_per_bit = 3;
  const int repeat_msg = 1;
  const int cycle_max = 100000000;

  uart_top<cycles_per_bit, repeat_msg> top;
  top.tock(0);

  //uint32_t checksum = 0;
  auto time_a = timestamp();
  for (int cycle = 0; cycle < cycle_max; cycle++) {
    top.tock(1);
    //checksum += top.data();
  }
  auto time_b = timestamp();

  double delta_sec = (double(time_b) - double(time_a)) / 1000000000.0;
  double rate = double(cycle_max) / delta_sec;
  printf("Simulation rate %f Mhz\n", rate / 1000000.0);
  //printf("Checksum 0x%08x\n", checksum);
 }

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
        break;
      }
    }
  }

  benchmark();

  return -1;
}
